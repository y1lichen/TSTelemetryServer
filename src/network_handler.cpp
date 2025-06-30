#include "network_handler.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>

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

    std::memset(&address, 0, sizeof(sockaddr));
    m_port = PORT;

    m_send_timeout.tv_sec = TIMEOUT_SEND_SEC;
    m_send_timeout.tv_usec = 0;

    m_recv_timeout.tv_sec = TIMEOUT_SEND_SEC;
    m_recv_timeout.tv_usec = 0;

    m_select_timeout.tv_sec = 0;
    m_select_timeout.tv_usec = 0;

    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<u_short>(m_port));
    address.sin_addr.s_addr = INADDR_ANY;

    m_topSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_topSocket == 0) {
        throw std::runtime_error("Unable to create socket!");
    }

    int flag = 1;
    if (setsockopt(m_topSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&flag), sizeof(int)) < 0) {
        throw std::runtime_error("Unable to set socket options!");
    }

    if (bind(m_topSocket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        throw std::runtime_error("Unable to bind to address!");
    }

    if (listen(m_topSocket, MAX_CLIENTS) < 0) {
        throw std::runtime_error("Unable to listen on socket!");
    }
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

int NetworkHandler::sendMessage(SOCKET socket, const char* msg, ssize_t size) {
    ssize_t sent = send(socket, msg, static_cast<int>(size), 0);
    if (sent == -1) return -1;

    while (sent < size) {
        ssize_t remainder = size - sent;
        ssize_t sentNext = send(socket, msg + sent, static_cast<int>(remainder), 0);
        if (sentNext == -1) return -1;
        sent += sentNext;
    }
    return 0;
}

void NetworkHandler::fdReset() {
    FD_ZERO(&m_subscriberSet);
    FD_SET(m_topSocket, &m_subscriberSet);
    m_maxSocket = m_topSocket;

    for (SOCKET s : m_subscribers) {
        FD_SET(s, &m_subscriberSet);
#ifndef _WIN32
        m_maxSocket = std::max(m_maxSocket, s);
#endif
    }
}

void NetworkHandler::newConnection() {
    sockaddr_in incoming;
    socklen_t addrlen = sizeof(incoming);
    SOCKET newSocket = accept(m_topSocket, reinterpret_cast<struct sockaddr*>(&incoming), &addrlen);

    if (newSocket == 0 || m_subscribers.size() >= MAX_CLIENTS) {
        CLOSE_SOCKET(newSocket);
        return;
    }

    setsockopt(newSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&m_send_timeout), sizeof(m_send_timeout));
    setsockopt(newSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&m_recv_timeout), sizeof(m_recv_timeout));

    const char* event = m_lastSentFrame.c_str();
    size_t event_size = std::strlen(event) + 1;
    if (sendMessage(newSocket, event, static_cast<ssize_t>(event_size)) == -1) {
        CLOSE_SOCKET(newSocket);
        return;
    }

    m_subscribers.push_back(newSocket);
}

void NetworkHandler::checkDeadConnections() {
    std::vector<SOCKET> deadSockets;
    for (SOCKET s : m_subscribers) {
        if (FD_ISSET(s, &m_subscriberSet)) {
            char buffer[16];
            if (recv(s, buffer, 8, MSG_PEEK) == 0) {
                CLOSE_SOCKET(s);
                deadSockets.push_back(s);
            }
        }
    }
    for (SOCKET s : deadSockets) {
        m_subscribers.remove(s);
    }
}

void NetworkHandler::checkQueue() {
    while (!m_eventQueue->IsEmpty()) {
        EventInfo poppedEvent = m_eventQueue->PopEvent();
        if (poppedEvent.type.empty()) break;

        const char* event = poppedEvent.event.c_str();
        size_t event_size = std::strlen(event) + 1;

        std::vector<SOCKET> deadSockets;
        for (SOCKET s : m_subscribers) {
            if (sendMessage(s, event, static_cast<ssize_t>(event_size)) == -1) {
                CLOSE_SOCKET(s);
                deadSockets.push_back(s);
            }
        }
        for (SOCKET s : deadSockets) {
            m_subscribers.remove(s);
        }
        if (poppedEvent.type == EVENT_FRAME) {
            m_lastSentFrame = poppedEvent.event;
        }
    }
}

void NetworkHandler::EventLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        fdReset();
        int result = select(static_cast<int>(m_maxSocket + 1), &m_subscriberSet, nullptr, nullptr, &m_select_timeout);
        if (result < 0) {
            throw std::runtime_error("Error during select in the network code!");
        }
        if (FD_ISSET(m_topSocket, &m_subscriberSet)) {
            newConnection();
        }
        checkDeadConnections();
        checkQueue();
    }
}

void NetworkHandler::Cleanup() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    delete m_instance;
    m_instance = nullptr;
}

NetworkHandler::~NetworkHandler() {
    for (SOCKET s : m_subscribers) {
        CLOSE_SOCKET(s);
    }
    m_subscribers.clear();
    CLOSE_SOCKET(m_topSocket);
#ifdef _WIN32
    WSACleanup();
#endif
}
