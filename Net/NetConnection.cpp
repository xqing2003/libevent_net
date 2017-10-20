// ：

#include "NetConnection.h"

NetConnection::NetConnection() : m_oInputStream(&m_oSocket), m_oOutputStream(&m_oSocket), m_pEvent(nullptr) {
	;
}

NetConnection::~NetConnection() {
	closeConnection();
}

void NetConnection::closeConnection() {
	if (!isConnected()) {
		return;
	}
	m_oSocket.Close();
	releaseEvent();
	onClosed();
	m_bIsConnected = false;
}

bool NetConnection::sendPacket(Packet *p) {
	if (!isConnected()) {
		return false;
	}
	uint headLen = sizeof(PacketHead);
	uint packetLen = p->getPacketLen();
	uint needLen = headLen + packetLen;
	if (needLen > m_oOutputStream.GetFreeLength()) {
		bool ret = m_oOutputStream.Resize(m_oOutputStream.Capacity() + 1);
		if (!ret) {
			int ret = m_oOutputStream.Flush();
			if (ret <= 0) {
				return false;
			}
		}
	}
	if (needLen > m_oOutputStream.GetFreeLength()) {
		return false;
	}

	m_oOutputStream.StartTransaction(false);
	try
	{
		PacketHead head;
		head.msgId = p->getPacketId();
		head.msgLen = p->getPacketLen();
		m_oOutputStream.Write(head.msgId);
		m_oOutputStream.Write(head.msgLen);

		int curLen = m_oOutputStream.Length();
		p->write(m_oOutputStream);
		if (m_oOutputStream.Length() - curLen != p->getPacketLen()) {
			m_oOutputStream.RollBack(false);
			onSendPacketError(p);
			return false;
		}
	}
	catch (...)
	{
		m_oOutputStream.RollBack(false);
		onSendPacketError(p);
		return false;
	}
	return true;
}

int NetConnection::readSocketData() {
	if (!isConnected()) {
		return 0;
	}
	int nRead = m_oInputStream.Fill();
	return nRead;
}

int NetConnection::sendSocketData() {
	if (!isConnected()) {
		return 0;
	}
	return m_oOutputStream.Flush();
}

void NetConnection::initConnection(const CSocket &sock, event *pEvent) {
	m_oSocket = sock;
	m_pEvent = pEvent;
	onConnected();
}

void NetConnection::releaseEvent() {
	if (nullptr != m_pEvent) {
		event_del(m_pEvent);
		event_free(m_pEvent);
		m_pEvent = nullptr;
	}
}