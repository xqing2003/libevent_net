#pragma once
// ：

#include "NetManager.h"
#include "NetConnection.h"
#include "ConnectionManager.h"

#include <map>
using namespace std;
typedef event_callback_fn NetEventCallBack;

class TCPServer
{
public:
public:
	TCPServer(NetEventCallBack onConnect, NetEventCallBack onRead, NetEventCallBack onWrite);
	~TCPServer();
public:
	bool listen(uint16 port);
	void tick(int64 time);
	void exit();
	void setConnectionMgr(ConnectionManager *pMgr) { m_pConnectMgr = pMgr; } // TCP Server Destroy connection manager
	ConnectionManager *getConnectionMgr() { return m_pConnectMgr; }
protected:
	CSocket m_oListenSocket;
	NetManager m_oNetMgr;
	ConnectionManager *m_pConnectMgr;
	NetEventCallBack m_fnOnConnectable;
	NetEventCallBack m_fnOnReadable;
	NetEventCallBack m_fnOnWritable;
};