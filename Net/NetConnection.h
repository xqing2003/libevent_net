#pragma once
// ：
#include "CSocket.h"
#include "Address.h"
#include "SocketStream.h"
#include "Packet.h"
#include "event2/event.h"

class NetConnection
{
public:
	NetConnection();
	virtual ~NetConnection();

public:
	void closeConnection();
	//int  send(const char *buf, size_t len);
	bool sendPacket(Packet *p);
	bool isConnected() const { return m_bIsConnected; }
	int  readSocketData();
	int  sendSocketData();
	void initConnection(const CSocket &sock, event *pEvent);

	void setConnectId(int id) {
		m_nId = id;
	}
	int  getConnectId() const { return m_nId; }

public: 
	virtual void onConnected() {}
	virtual void onClosed() {}
	virtual void onConnectionError() { closeConnection(); }
	virtual void onSendPacketError(const Packet *p) {}
protected:
	void releaseEvent();
	void setEvent(event *pEvent)  { m_pEvent = pEvent; }
	event *getEvent() { return m_pEvent; }
protected:
	CSocket m_oSocket;
	SocketStream m_oInputStream;
	SocketStream m_oOutputStream;
	event *m_pEvent;
	bool    m_bIsConnected;
	int    m_nId;
};