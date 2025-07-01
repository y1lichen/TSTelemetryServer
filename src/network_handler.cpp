#include "network_handler.h"
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <iostream>

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

NetworkHandler* NetworkHandler::m_instance = nullptr;
std::mutex NetworkHandler::m_instanceMutex;

NetworkHandler::NetworkHandler(EventQueue* eventQueue) {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        throw std::runtime_error("Windows Sockets failed to initialize!");
    }
#endif
    
    if (!eventQueue) {
        throw std::runtime_error("Nonexistent event queue!");
    }
    m_eventQueue = eventQueue;

    // 初始化地址結構
    std::memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    std::memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    
    m_port = PORT;

    // 設置超時時間
    m_send_timeout.tv_sec = TIMEOUT_SEND_SEC;
    m_send_timeout.tv_usec = 0;
    m_recv_timeout.tv_sec = 1; // 1秒接收超時
    m_recv_timeout.tv_usec = 0;
    m_select_timeout.tv_sec = 0;
    m_select_timeout.tv_usec = 100000; // 100ms

    // 配置服務器地址
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(static_cast<unsigned short>(m_port));
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 創建UDP socket
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket == INVALID_SOCKET) {
        throw std::runtime_error("Unable to create UDP socket!");
    }

    // 設置socket選項
#ifdef _WIN32
    DWORD timeout = TIMEOUT_SEND_SEC * 1000; // Windows使用毫秒
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set send timeout!");
    }
    timeout = 1000; // 1秒接收超時
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set receive timeout!");
    }
#else
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, &m_send_timeout, sizeof(m_send_timeout)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set send timeout!");
    }
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &m_recv_timeout, sizeof(m_recv_timeout)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set receive timeout!");
    }
#endif

    // 綁定socket
    if (bind(m_socket, reinterpret_cast<struct sockaddr*>(&m_serverAddr), sizeof(m_serverAddr)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to bind to address!");
    }

    m_hasClient = false;
    m_lastClientActivity = 0;
}

std::jthread* NetworkHandler::GetEventThread(EventQueue* eventQueue) {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (!m_instance) {
        m_instance = new NetworkHandler(eventQueue);
    }
    return new std::jthread([](std::stop_token st) {
        m_instance->EventLoop(st);
    });
}

int NetworkHandler::sendMessage(const char* msg, size_t size) {
    if (!m_hasClient) {
        return -1; // 沒有客戶端
    }

    ssize_t sent = sendto(m_socket, msg, static_cast<int>(size), 0, 
                         reinterpret_cast<struct sockaddr*>(&m_clientAddr), m_clientAddrLen);
    
    if (sent < 0) {
        // 發送失敗，可能客戶端已斷線
        resetClient();
        return -1;
    }
    
    return 0;
}

void NetworkHandler::handleIncomingData() {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    
    ssize_t received = recvfrom(m_socket, buffer, BUFFER_SIZE - 1, 0, 
                               reinterpret_cast<struct sockaddr*>(&fromAddr), &fromAddrLen);
    
    if (received > 0) {
        buffer[received] = '\0';
        
        // 檢查是否是新客戶端或現有客戶端
        bool isNewClient = false;
        if (!m_hasClient) {
            // 沒有客戶端，接受新連接
            m_clientAddr = fromAddr;
            m_clientAddrLen = fromAddrLen;
            m_hasClient = true;
            isNewClient = true;
            std::cout << "New UDP client connected from " 
                      << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
        } else if (fromAddr.sin_addr.s_addr == m_clientAddr.sin_addr.s_addr && 
                   fromAddr.sin_port == m_clientAddr.sin_port) {
            // 來自現有客戶端的數據
            isNewClient = false;
        } else {
            // 來自其他客戶端，拒絕（因為只允許一個客戶端）
            std::cout << "Rejected connection from " 
                      << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) 
                      << " (only one client allowed)" << std::endl;
            return;
        }
        
        // 更新客戶端活動時間
        m_lastClientActivity = time(nullptr);
        
        // 如果是新客戶端且有上次發送的幀，則發送給它
        if (isNewClient && !m_lastSentFrame.empty()) {
            sendMessage(m_lastSentFrame.c_str(), m_lastSentFrame.length() + 1);
        }
        
        // 處理接收到的消息（如果需要的話）
        // 這裡可以根據需要處理客戶端發送的數據
    }
}

void NetworkHandler::checkClientTimeout() {
    if (m_hasClient) {
        time_t currentTime = time(nullptr);
        if (currentTime - m_lastClientActivity > CLIENT_TIMEOUT_SEC) {
            std::cout << "Client timeout, disconnecting..." << std::endl;
            resetClient();
        }
    }
}

void NetworkHandler::checkQueue() {
    while (!m_eventQueue->IsEmpty()) {
        EventInfo poppedEvent = m_eventQueue->PopEvent();
        if (poppedEvent.type.empty()) break;

        const char* event = poppedEvent.event.c_str();
        size_t event_size = poppedEvent.event.length() + 1;

        if (m_hasClient) {
            if (sendMessage(event, event_size) == -1) {
                // 發送失敗，客戶端可能已斷線
                resetClient();
            }
        }
        
        // 保存最後發送的幀
        if (poppedEvent.type == EVENT_FRAME) {
            m_lastSentFrame = poppedEvent.event;
        }
    }
}

bool NetworkHandler::isClientValid() {
    return m_hasClient;
}

void NetworkHandler::resetClient() {
    m_hasClient = false;
    m_lastClientActivity = 0;
    std::memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    m_clientAddrLen = sizeof(m_clientAddr);
}

void NetworkHandler::EventLoop(std::stop_token stopToken) {
    using namespace std::chrono;
    auto lastSend = steady_clock::now();
    auto lastTimeoutCheck = steady_clock::now();

    std::cout << "UDP NetworkHandler started on port " << m_port << std::endl;

    while (!stopToken.stop_requested()) {
        // 使用select檢查是否有數據可讀
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(m_socket, &readSet);

#ifdef _WIN32
        int result = select(0, &readSet, nullptr, nullptr, &m_select_timeout);
#else
        int result = select(m_socket + 1, &readSet, nullptr, nullptr, &m_select_timeout);
#endif

        if (result < 0) {
            throw std::runtime_error("Error during select!");
        }
        
        if (result > 0 && FD_ISSET(m_socket, &readSet)) {
            handleIncomingData();
        }

        auto now = steady_clock::now();
        
        // 檢查客戶端超時（每5秒檢查一次）
        if (duration_cast<seconds>(now - lastTimeoutCheck).count() >= 5) {
            checkClientTimeout();
            lastTimeoutCheck = now;
        }

        // 傳輸最小間隔 30 毫秒
        if (duration_cast<milliseconds>(now - lastSend).count() >= 30) {
            checkQueue();
            lastSend = now;
        }
    }
    
    std::cout << "UDP NetworkHandler stopped" << std::endl;
}

void NetworkHandler::Cleanup() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

NetworkHandler::~NetworkHandler() {
    if (m_socket != INVALID_SOCKET) {
        CLOSE_SOCKET(m_socket);
    }
#ifdef _WIN32
    WSACleanup();
#endif
}