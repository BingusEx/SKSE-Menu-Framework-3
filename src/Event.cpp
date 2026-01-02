#include "Event.h"

int64_t autoIncrement = 0;
static std::map<int64_t, Event::EventCallback> listeners;

int64_t Event::AddEventListener(EventCallback callback) {
    if (!callback) return 0;
    int64_t id = ++autoIncrement;
    listeners[id] = callback;
    return id;
}

void Event::RemoveEventListener(int64_t id) { listeners.erase(id); }

void Event::DispatchEvent(EventType type) {
    if (type == kNone) return;

    for (auto& it : listeners) {
        if (!it.second) continue;
        it.second(type);
    }
}
