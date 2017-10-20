#pragma once
// ：

#include "NetManager.h"
#include "NetConnection.h"

#include <map>
using namespace std;

class TCPServer
{
public:
	typedef event_callback_fn NetEventCallBack;
public:
	TCPServer(NetEventCallBack *pOnConnect, NetEventCallBack *pOnRead, NetEventCallBack *pOnWrite);
	~TCPServer();
public:
	bool listen(uint16 port);
	void tick(int64 time);
	void exit();
public:
	uint getConnectionNum() const { return m_oConnectionMap.size(); }
	NetConnection *getConnection(int id);

protected:
	CSocket m_oListenSocket;
	NetManager m_oNetMgr;
	map<int, NetConnection *> m_oConnectionMap;
	NetEventCallBack *m_pOnConnectable;
	NetEventCallBack *m_pOnReadable;
	NetEventCallBack *m_pOnWritable;
};