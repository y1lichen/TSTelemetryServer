#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#define EVENT_FRAME "frame"
#define EVENT_GAMEPLAY "gameplay"

struct EventInfo {
    std::string event;
    std::string type;
};

class EventQueue {
public:
    void PushEvent(std::string eventInfo, const char* type);
    EventInfo PopEvent();
    EventInfo PopEventBlocking();
    bool IsEmpty();

private:
    std::queue<EventInfo> m_serializedEvents;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
