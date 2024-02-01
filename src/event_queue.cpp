/*
This file is part of TSTelemetryServer.

Copyright (C) 2024 OrkenWhite.

TSTelemetryServer is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TSTelemetryServer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TSTelemetryServer. If not, see <https://www.gnu.org/licenses/>. 
*/

#include "event_queue.h"

void EventQueue::PushEvent(std::string eventInfo,const char* type){
    m_mutex.lock();
    EventInfo event;
    event.type = type;
    event.event = eventInfo;
    m_serializedEvents.push(event);
    m_mutex.unlock();
}

EventInfo EventQueue::PopEvent(){
    m_mutex.lock();
    if(m_serializedEvents.empty()){
        return {};
    }
    EventInfo event = m_serializedEvents.front();
    m_serializedEvents.pop();
    m_mutex.unlock();
    return event;
}

bool EventQueue::IsEmpty(){
    m_mutex.lock();
    bool isEmpty = m_serializedEvents.empty();
    m_mutex.unlock();
    return isEmpty;
}