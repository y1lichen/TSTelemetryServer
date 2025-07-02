#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <thread>
#include <mutex>
#include <string>
#include <atomic>
#include <stop_token>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #define CLOSE_SOCKET(s) closesocket(s)
  #define INVALID_SOCKET_VALUE INVALID_SOCKET
#else
  #include <unistd.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <fcntl.h>
  #include <errno.h>
  #define CLOSE_SOCKET(s) close(s)
  #define INVALID_SOCKET_VALUE -1
#endif

#define PORT 25555
#define BUFFER_SIZE 65535
#define TIMEOUT_SEND_SEC 1
#define CLIENT_TIMEOUT_SEC 10

// --- forward declarations for external global access ---
class TelemetryFrame;
class AbstractTelemetrySerializer;

class NetworkHandler {
public:
    static std::jthread* GetEventThread();
    static void Cleanup();

    ~NetworkHandler();

private:
    static NetworkHandler* m_instance;
    static std::mutex m_instanceMutex;

    NetworkHandler(); // No EventQueue required anymore
    void EventLoop(std::stop_token stopToken);

    void handleIncomingData();
    void handleClientRequest(const std::string& request);
    void checkClientTimeout();
    void resetClient();
    bool isClientValid();

    int sendMessage(const char* msg, size_t size);

    #ifdef _WIN32
        SOCKET m_socket = INVALID_SOCKET;
    #else
        int m_socket = -1;
    #endif

    int m_port;
    bool m_hasClient;
    time_t m_lastClientActivity;

    struct sockaddr_in m_serverAddr{};
    struct sockaddr_in m_clientAddr{};
    socklen_t m_clientAddrLen{};

    struct timeval m_send_timeout{};
    struct timeval m_recv_timeout{};
    struct timeval m_select_timeout{};

#ifdef _WIN32
    WSADATA m_wsaData{};
#endif
};

#endif // NETWORK_HANDLER_H
