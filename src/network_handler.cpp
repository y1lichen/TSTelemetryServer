#include "network_handler.h"
#include "json_telemetry_serializer.h"

#include <stdexcept>
#include <cstring>
#include <chrono>
#include <iostream>

// 全域變數來自其他模組
extern TelemetryFrame telemetryData;
extern AbstractTelemetrySerializer* serializer;

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#endif

NetworkHandler* NetworkHandler::m_instance = nullptr;
std::mutex NetworkHandler::m_instanceMutex;

NetworkHandler::NetworkHandler() {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        throw std::runtime_error("Windows Sockets failed to initialize!");
    }
#endif

    std::memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    std::memset(&m_clientAddr, 0, sizeof(m_clientAddr));

    m_port = PORT;

    m_send_timeout.tv_sec = TIMEOUT_SEND_SEC;
    m_send_timeout.tv_usec = 0;
    m_recv_timeout.tv_sec = 0;
    m_recv_timeout.tv_usec = 100000; // 100ms
    m_select_timeout.tv_sec = 0;
    m_select_timeout.tv_usec = 10000; // 10ms

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(static_cast<unsigned short>(m_port));
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    if (m_socket == INVALID_SOCKET) {
        throw std::runtime_error("Unable to create UDP socket!");
    }
#else
    if (m_socket < 0) {
        throw std::runtime_error("Unable to create UDP socket!");
    }
#endif

#ifdef _WIN32
    u_long mode = 1;
    if (ioctlsocket(m_socket, FIONBIO, &mode) != 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set non-blocking mode!");
    }
#else
    int flags = fcntl(m_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(m_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to set non-blocking mode!");
    }
#endif

    int bufferSize = 64 * 1024;
#ifdef _WIN32
    setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize));
    setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&bufferSize), sizeof(bufferSize));
#else
    setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
#endif

    if (bind(m_socket, reinterpret_cast<struct sockaddr*>(&m_serverAddr), sizeof(m_serverAddr)) < 0) {
        CLOSE_SOCKET(m_socket);
        throw std::runtime_error("Unable to bind to address!");
    }

    m_hasClient = false;
    m_lastClientActivity = 0;
}

std::jthread* NetworkHandler::GetEventThread() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (!m_instance) {
        m_instance = new NetworkHandler();
    }
    return new std::jthread([](std::stop_token st) {
        m_instance->EventLoop(st);
    });
}

int NetworkHandler::sendMessage(const char* msg, size_t size) {
    if (!m_hasClient) return -1;

    ssize_t sent = sendto(m_socket, msg, static_cast<int>(size), 0,
                          reinterpret_cast<struct sockaddr*>(&m_clientAddr), m_clientAddrLen);
    if (sent < 0) {
#ifdef _WIN32
        int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK) {
            resetClient();
            return -1;
        }
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            resetClient();
            return -1;
        }
#endif
        return 0;
    }

    return 0;
}

void NetworkHandler::handleIncomingData() {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);

    while (true) {
        ssize_t received = recvfrom(m_socket, buffer, BUFFER_SIZE - 1, 0,
                                    reinterpret_cast<struct sockaddr*>(&fromAddr), &fromAddrLen);
        if (received <= 0) {
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) break;
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
#endif
            break;
        }

        buffer[received] = '\0';

        if (!m_hasClient) {
            m_clientAddr = fromAddr;
            m_clientAddrLen = fromAddrLen;
            m_hasClient = true;
            std::cout << "New UDP client connected from "
                      << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << std::endl;
        } else if (fromAddr.sin_addr.s_addr != m_clientAddr.sin_addr.s_addr ||
                   fromAddr.sin_port != m_clientAddr.sin_port) {
            continue;
        }

        m_lastClientActivity = time(nullptr);

        handleClientRequest(std::string(buffer));
    }
}

void NetworkHandler::handleClientRequest(const std::string& request) {
    if (request == "REQUEST_FRAME") {
        if (serializer) {
            std::string frameData = serializer->SerializeFrame(&telemetryData);
            sendMessage(frameData.c_str(), frameData.length() + 1);
        }
    } else if (request == "PING") {
        std::string response = "PONG";
        sendMessage(response.c_str(), response.length() + 1);
    } else {
        std::string response = "UNKNOWN_REQUEST";
        sendMessage(response.c_str(), response.length() + 1);
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
    auto lastTimeoutCheck = steady_clock::now();

    std::cout << "UDP NetworkHandler (Pull Mode) started on port " << m_port << std::endl;

    while (!stopToken.stop_requested()) {
        handleIncomingData();

        auto now = steady_clock::now();
        if (duration_cast<seconds>(now - lastTimeoutCheck).count() >= 5) {
            checkClientTimeout();
            lastTimeoutCheck = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "UDP NetworkHandler (Pull Mode) stopped" << std::endl;
}

void NetworkHandler::Cleanup() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

NetworkHandler::~NetworkHandler() {
    if (
#ifdef _WIN32
        m_socket != INVALID_SOCKET
#else
        m_socket >= 0
#endif
    ) {
        CLOSE_SOCKET(m_socket);
    }
#ifdef _WIN32
    WSACleanup();
#endif
}
