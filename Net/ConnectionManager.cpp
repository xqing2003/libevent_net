// ：
#include "BaseTypeDef.h"
#include "ConnectionManager.h"

ConnectionManager::ConnectionManager() : m_nConnectionId(-1) {

}

ConnectionManager::~ConnectionManager() {
	for (auto &kv : m_oConnectionMap) {
		SAFE_DELETE(kv.second);
	}
	m_oConnectionMap.clear();
}

void ConnectionManager::addConnection(NetConnection *pConn) {
	++m_nConnectionId;
	pConn->setConnectId(m_nConnectionId);
	m_oConnectionMap[m_nConnectionId] = pConn;
}

void ConnectionManager::removeConnection(int connId) {
	m_oConnectionMap.erase(connId);
}

NetConnection *ConnectionManager::getConnection(int id) {
	map<int, NetConnection *>::iterator it = m_oConnectionMap.find(id);
	if (it == m_oConnectionMap.end()) {
		return nullptr;
	}
	return it->second;
}



