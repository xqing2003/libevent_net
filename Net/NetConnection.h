#pragma once
// ：
#include "CSocket.h"
#include "Address.h"
#include "SocketStream.h"
#include "Packet.h"

class NetConnection
{
public:
	NetConnection();
	virtual ~NetConnection();

public:
	void closeConnection();
	int  send(const char *buf, size_t len);
	bool sendPacket(Packet *p);
	bool isConnected() const { return m_bIsConnected; }
public: 
	virtual void onConnectReadable();
	virtual void onConnectWritable();
	virtual void onConnected();
	virtual void onClosed() {}
	virtual void onConnectionError() { closeConnection(); }

protected:
	CSocket m_oSocket;
	SocketStream m_oInputStream;
	SocketStream m_oOutputStream;

	bool    m_bIsConnected;
};