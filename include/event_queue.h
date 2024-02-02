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


#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <queue>
#include <string>
#include <mutex>

#define EVENT_FRAME "frame"
#define EVENT_GAMEPLAY "gameplay"

struct EventInfo{
        std::string event;
        std::string type;
};

class EventQueue
{
public:
        void PushEvent(std::string eventInfo,const char* type);
        EventInfo PopEvent();
        bool IsEmpty();
private:
        std::queue<EventInfo> m_serializedEvents;
        std::mutex m_mutex;
};

#endif
