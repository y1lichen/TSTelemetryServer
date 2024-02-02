/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published by the 
Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU Lesser General Public License for more details.

You should have received a copy of the 
GNU Lesser General Public License along with TSTelemetryServer. 
If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "event_queue.h"

#include <list>
#include <string>
#include <thread>
#include <stop_token>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSE_SOCKET closesocket
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
#define TIMEOUT_USEC 1000
#define PORT 3101

class NetworkHandler{
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
                NetworkHandler(EventQueue* queue);
                ~NetworkHandler();
                EventQueue* m_eventQueue;
                int m_port;
                struct timeval m_timeout;
                struct sockaddr_in address;
                fd_set m_subscriberSet;
                std::string m_lastSentFrame;
                void newConnection();
                void checkDeadConnections();
                void checkQueue();
                void fdReset();
                static NetworkHandler* m_instance;
};
#endif
