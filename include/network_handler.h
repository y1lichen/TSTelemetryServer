#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "event_queue.h"

#include <string>
#include <thread>
#include <stop_token>
#include <mutex>
#include <atomic>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSE_SOCKET closesocket
#include <stddef.h>
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long ssize_t;
#endif
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET int
#define CLOSE_SOCKET close
#endif

#define TIMEOUT_SEND_SEC 5
#define PORT 3101
#define BUFFER_SIZE 1024
#define CLIENT_TIMEOUT_SEC 30

class NetworkHandler {
public:
    static std::jthread* GetEventThread(EventQueue* queue);
    void EventLoop(std::stop_token stopToken);
    static void Cleanup();

private:
#ifdef _WIN32
    WSAData m_wsaData;
#endif
    SOCKET m_socket = INVALID_SOCKET;
    EventQueue* m_eventQueue = nullptr;
    int m_port;

    struct timeval m_send_timeout;
    struct timeval m_recv_timeout;
    struct timeval m_select_timeout;

    struct sockaddr_in m_serverAddr;
    struct sockaddr_in m_clientAddr;
    socklen_t m_clientAddrLen = sizeof(m_clientAddr);
    
    std::atomic<bool> m_hasClient{false};
    std::atomic<time_t> m_lastClientActivity{0};
    std::string m_lastSentFrame;

    static NetworkHandler* m_instance;
    static std::mutex m_instanceMutex;

    NetworkHandler(EventQueue* queue);
    ~NetworkHandler();

    int sendMessage(const char* msg, size_t size);
    void handleIncomingData();
    void checkClientTimeout();
    void checkQueue();
    bool isClientValid();
    void resetClient();
};

#endif // NETWORK_HANDLER_H