#include "event_queue.h"

void EventQueue::PushEvent(std::string eventInfo, const char* type) {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (std::string(type) == "frame") {
        // 移除舊的 frame
        std::queue<EventInfo> filtered;
        while (!m_serializedEvents.empty()) {
            EventInfo current = m_serializedEvents.front();
            m_serializedEvents.pop();
            if (current.type != "frame")
                filtered.push(current);
        }
        std::swap(m_serializedEvents, filtered);
    }

    m_serializedEvents.push({eventInfo, type});
    m_cond.notify_one();
}

EventInfo EventQueue::PopEvent() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_serializedEvents.empty()) {
        return {};
    }
    EventInfo event = m_serializedEvents.front();
    m_serializedEvents.pop();
    return event;
}

EventInfo EventQueue::PopEventBlocking() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, [this]() { return !m_serializedEvents.empty(); });

    EventInfo event = m_serializedEvents.front();
    m_serializedEvents.pop();
    return event;
}

bool EventQueue::IsEmpty() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_serializedEvents.empty();
}
