#pragma once
// ：
#include "CSocket.h"

class NetConnection
{
public:
	NetConnection();
	virtual ~NetConnection();

public:
	void closeConnection();

public: 
	virtual void onConnected() {}
	virtual void onClosed() {}

protected:
	CSocket m_oSocket;
	bool    m_bIsConnected;
};