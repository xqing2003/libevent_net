// ：

#include "TCPServer.h"

TCPServer::TCPServer(NetEventCallBack *pOnConnect, NetEventCallBack *pOnRead, NetEventCallBack *pOnWrite) :
					m_pOnConnectable(pOnConnect), m_pOnReadable(pOnRead), m_pOnWritable(pOnWrite) {
	m_oNetMgr.init();
}

TCPServer::~TCPServer() {
	m_oNetMgr.exit();
}

bool TCPServer::listen(uint16 port) {
	bool ret = m_oListenSocket.Create();
	if (!ret) {
		return false;
	}

	ret = m_oListenSocket.Bind(port);
	if (!ret) {
		return false;
	}

	m_oNetMgr.addListenSocket(m_oListenSocket.GetSocket(), *m_pOnConnectable, this);
	return true;
}

void TCPServer::tick(int64 time) {
	m_oNetMgr.loop(0);
}

void TCPServer::exit() {
	m_oNetMgr.exit();
}