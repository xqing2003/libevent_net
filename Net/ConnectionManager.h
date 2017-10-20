#pragma once
// ：

#include "NetConnection.h"

#include <map>
using namespace std;

class ConnectionManager
{
public:
	ConnectionManager();
	virtual ~ConnectionManager();
public:
	void addConnection(NetConnection *pConn);
	void removeConnection(int connId);
	uint getConnectionNum() const { return m_oConnectionMap.size(); }
	NetConnection *getConnection(int id);
protected:
	map<int, NetConnection *> m_oConnectionMap;
	int m_nConnectionId;
};