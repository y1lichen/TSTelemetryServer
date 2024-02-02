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


#include "network_handler.h"
#include <stdexcept>
#include <string.h>
#include <algorithm>


/*
 * Stop the linker from whining on Windows
 */
NetworkHandler* NetworkHandler::m_instance = nullptr;

NetworkHandler::NetworkHandler(EventQueue* eventQueue){
    #ifdef _WIN32
    if(WSAStartup(MAKEWORD(2,2),&m_wsaData) != 0){
        throw std::runtime_error("Windows Sockets failed to initialize!");
    }
    #endif
    if(eventQueue == nullptr){
        throw std::runtime_error("Nonexistent event queue!");
    }
    m_eventQueue = eventQueue;
    memset(&address,0,sizeof(sockaddr));
    m_port = PORT;
    m_timeout.tv_usec = TIMEOUT_USEC;
    address.sin_family = AF_INET;
    address.sin_port = htons(m_port);
    address.sin_addr.s_addr = INADDR_ANY;
    m_topSocket = socket(AF_INET,SOCK_STREAM,0);
    int flag = 1;
    int lastError = setsockopt(m_topSocket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(int));
    if(lastError < 0){
        throw std::runtime_error("Unable to set socket options!");
    }
    lastError = bind(m_topSocket,(struct sockaddr*) &address,sizeof(address));
    if(lastError < 0){
        throw std::runtime_error("Unable to bind to address!");
    }
    lastError = listen(m_topSocket,MAX_CLIENTS);
    if(lastError < 0){
        throw std::runtime_error("Unable to listen on socket!");
    }
}

std::jthread* NetworkHandler::GetEventThread(EventQueue* eventQueue){
    if(m_instance == nullptr){
        m_instance = new NetworkHandler(eventQueue);
    }
    return new std::jthread([](std::stop_token st){m_instance->EventLoop(st);});
}

void NetworkHandler::fdReset(){
    FD_ZERO(&m_subscriberSet);
    FD_SET(m_topSocket,&m_subscriberSet);
    m_maxSocket = m_topSocket;
    for(SOCKET s : m_subscribers){
        FD_SET(s,&m_subscriberSet);
        #ifndef _WIN32
        m_maxSocket = s > m_maxSocket ? s : m_maxSocket; 
        #endif
    }
}

void NetworkHandler::newConnection(){
    sockaddr_in incoming;
    socklen_t addrlen = sizeof(incoming);
    SOCKET newSocket = accept(m_topSocket,(struct sockaddr*) &incoming,&addrlen);
    if(m_subscribers.size() > MAX_CLIENTS){
        CLOSE_SOCKET(newSocket);
        return;
    }
    if(newSocket > 0){
        m_subscribers.push_back(newSocket);
        const char* event = m_lastSentFrame.c_str();
        size_t event_size = strlen(event) + 1;
        send(newSocket,event,event_size,0);
    }
}

void NetworkHandler::checkDeadConnections(){
    std::vector<SOCKET> deadSockets;
    for(SOCKET s : m_subscribers){
        if(FD_ISSET(s,&m_subscriberSet)){
            char* buffer = (char*) malloc(16);
            if(recv(s,buffer,8,0) == 0){
                CLOSE_SOCKET(s);
                deadSockets.push_back(s);
            }
            free(buffer);
        }
    }
    for(SOCKET s : deadSockets){
        m_subscribers.remove(s);
    }
}

void NetworkHandler::checkQueue(){
    while(!m_eventQueue->IsEmpty()){
        EventInfo poppedEvent = m_eventQueue->PopEvent();
        if(poppedEvent.type == ""){
            break;
        }
        const char* event = poppedEvent.event.c_str();
        size_t event_size = strlen(event) + 1;
        for(SOCKET s : m_subscribers){
            send(s,event,event_size,0);
        }
        if(poppedEvent.type == EVENT_FRAME)
        {
            m_lastSentFrame = poppedEvent.event;
        }
    }
}

void NetworkHandler::EventLoop(std::stop_token stopToken){
    while(!stopToken.stop_requested()){
        fdReset();
        int lastError = select(m_maxSocket+1,&m_subscriberSet,NULL,NULL,&m_timeout);
        /*
         * This is present mostly for debugging purposes
         */
        if(lastError < 0){
            throw std::runtime_error("Error during select in the network code!");
        }
        if(FD_ISSET(m_topSocket,&m_subscriberSet)){
            newConnection();
        }
        checkDeadConnections();
        checkQueue();
    }
}



/*
 * DO NOT call this anywhere other than shutdown!
 */
void NetworkHandler::Cleanup(){
    if(m_instance != nullptr){
        delete m_instance;
        m_instance = nullptr;
    }
}

NetworkHandler::~NetworkHandler(){
    for(SOCKET s : m_subscribers){
        CLOSE_SOCKET(s);
        m_subscribers.remove(s);
    }
    CLOSE_SOCKET(m_topSocket);
    #ifdef _WIN32
    WSACleanup();
    #endif
}
