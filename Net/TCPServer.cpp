// ：

#include "TCPServer.h"

TCPServer::TCPServer(NetEventCallBack onConnect, NetEventCallBack onRead, NetEventCallBack onWrite) :
					m_fnOnConnectable(onConnect), m_fnOnReadable(onRead), m_fnOnWritable(onWrite) {
	m_pConnectMgr = nullptr;
	m_oNetMgr.init();
}

TCPServer::~TCPServer() {
	m_oNetMgr.exit();
	SAFE_DELETE(m_pConnectMgr);
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

	ret = m_oListenSocket.Listen(port);
	if (!ret) {
		return false;
	}

	m_oListenSocket.SetNonBlocking(true);

	m_oNetMgr.addListenSocket(m_oListenSocket.GetSocket(), m_fnOnConnectable, this);
	return true;
}

void TCPServer::tick(int64 time) {
	m_oNetMgr.loop(0);
}

void TCPServer::exit() {
	m_oNetMgr.exit();
}