#include "Event.h"

uint64_t autoIncrement = 0;
static std::map<uint64_t, Event::EventCallback> listeners;

uint64_t Event::AddEventListener(EventCallback callback) {
    if (!callback) return 0;
    uint64_t id = ++autoIncrement;
    listeners[id] = callback;
    return id;
}

void Event::RemoveEventListener(uint64_t id) { listeners.erase(id); }

void Event::DispatchEvent(EventType type) {
    if (type == kNone) return;

    for (auto& it : listeners) {
        if (!it.second) continue;
        it.second(type);
    }
}
