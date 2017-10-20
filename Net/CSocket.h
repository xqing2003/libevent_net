#pragma once

#include "NetDefine.h"
#include <string>

class CSocket {
public:
	CSocket();
	CSocket(const char *psczIP, uint port);
	CSocket(SOCKET s, const struct sockaddr_in *addr);

	virtual ~CSocket();
public:
	bool Create();
	void Close();
	bool Connect(const char *psczIP = nullptr, uint port = -1);
	bool Reconnect(const char *psczIP, uint port);
	bool Bind(uint port);
	bool Listen(int backlog);
	SOCKET Accept(struct sockaddr *addr, uint *addrLen);
	CSocket *Accept();

	int  Send(const void *buf, int len, uint flags = 0);
	int  Recv(void* buf, uint len, uint flags = 0);

	bool IsValidSocket() const;
	bool IsSocketError() const;
	SOCKET GetSocket() const;

	uint GetSendBuffSize() const;
	bool SetSendBuffSize(uint size);

	uint GetRecvBuffSize() const;
	bool SetRecvBuffSize(uint size);

	bool IsNonBlocking() const;
	bool SetNonBlocking(bool on = true);
	uint GetSockErrorID() const;
	std::string GetSockErrorMsg() const;

	uint GetAvaliable() const;
	bool GetSockOpt(int optname, void* optval, uint* optlen, int level = SOL_SOCKET);
	bool SetSockOpt(int optname, const void* optval, uint optlen, int level = SOL_SOCKET);
	const char *GetRemoteIP() const;
private:
	SOCKET m_nSocketID;
	SOCKADDR_IN m_oSockAddr;
	char m_szIP[IP_SIZE];
	uint m_nPort;
	bool m_bNoBlocking;
};

