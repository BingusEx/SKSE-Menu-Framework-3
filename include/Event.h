#pragma once
namespace Event {

    enum EventType {
		kNone = 0,
		kOpenMenu = 1,
		kCloseMenu = 2
	};

    typedef void(__stdcall* EventCallback)(EventType type);

    uint64_t AddEventListener(EventCallback callback);
    void RemoveEventListener(uint64_t id);
    void DispatchEvent(EventType type);
}