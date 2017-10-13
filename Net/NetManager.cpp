// ：
#include "NetManager.h"
#include <iostream>
using namespace std;

NetManager::NetManager() : m_pEventBase(nullptr), m_pListenEvent(nullptr){
	init();
}

NetManager::~NetManager() {
	exit();
}

bool NetManager::init() {
	
	if (nullptr == m_pEventBase) {
#if defined(_USEIOCP) && defined(__WINDOWS)
		struct event_config* cfg = event_config_new();
		event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
		//根据CPU实际数量配置libEvent的CPU数
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		event_config_set_num_cpus_hint(cfg, si.dwNumberOfProcessors);

		m_pEventBase = event_base_new_with_config(cfg);
		event_config_free(cfg);
#else
		m_pEventBase = event_base_new();
#endif
	}
	return true;
}

bool NetManager::loop(int deltaTime) {
	int result = event_base_loop(m_pEventBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	bool ret = false;
	
	switch (result)
	{
	case -1:
	case  1:
	{
		ret = false;
	}
	break;
	case 0:
	{
		ret = true;
	}
	break;
	default:
		break;
	}
	return ret;
}

void NetManager::exit() {
	event_base_loopexit(m_pEventBase, nullptr);

	if (nullptr != m_pListenEvent) {
		event_free(m_pListenEvent);
	}

	event_base_free(m_pEventBase);
	
	m_pEventBase = nullptr;
}

const char *NetManager::getIOModel() {
	return event_base_get_method(m_pEventBase);
}

void NetManager::addListenSocket(intptr_t sock, event_callback_fn callback, void *data) {
	short events = EV_READ | EV_PERSIST;

	m_pListenEvent = event_new(m_pEventBase, (intptr_t)sock, events, callback, data);
	event_add(m_pListenEvent, nullptr);
}



