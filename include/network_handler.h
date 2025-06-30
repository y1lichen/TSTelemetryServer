#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "event_queue.h"

#include <list>
#include <string>
#include <thread>
#include <stop_token>
#include <mutex>

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
#include <netinet/tcp.h>
#define INVALID_SOCKET -1
#define SOCKET int
#define CLOSE_SOCKET close
#endif

#define MAX_CLIENTS 8
#define TIMEOUT_SEND_SEC 5
#define PORT 3101

class NetworkHandler {
public:
    static std::jthread* GetEventThread(EventQueue* queue);
    void EventLoop(std::stop_token stopToken);
    static void Cleanup();

private:
#ifdef _WIN32
    WSAData m_wsaData;
#endif
    SOCKET m_maxSocket = INVALID_SOCKET;
    SOCKET m_topSocket = INVALID_SOCKET;
    std::list<SOCKET> m_subscribers;

    EventQueue* m_eventQueue;
    int m_port;

    struct timeval m_send_timeout;
    struct timeval m_recv_timeout;
    struct timeval m_select_timeout;

    struct sockaddr_in address;
    fd_set m_subscriberSet;

    std::string m_lastSentFrame;

    static NetworkHandler* m_instance;
    static std::mutex m_instanceMutex;

    NetworkHandler(EventQueue* queue);
    ~NetworkHandler();

    int sendMessage(SOCKET socket, const char* msg, ssize_t size);
    void newConnection();
    void checkDeadConnections();
    void checkQueue();
    void fdReset();
};

#endif // NETWORK_HANDLER_H
