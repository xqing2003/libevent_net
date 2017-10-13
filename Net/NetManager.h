#pragma once
// ：

#include "event2/event.h"

class NetManager
{
public:
	NetManager();
	~NetManager();
public:
	bool init();
	bool loop(int deltaTime);
	void exit();
	const char *getIOModel();
	void addListenSocket(intptr_t sock, event_callback_fn callback, void *data);

	event_base *getEventBase() { return m_pEventBase; }
private:
	event      *m_pListenEvent;
	event_base *m_pEventBase;
};
