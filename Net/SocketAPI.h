#ifndef _SOCKETAPI_H_
#define _SOCKETAPI_H_
#include "NetDefine.h"

#include <map>
#include <string>
using std::map;
using std::pair;
using std::string;

namespace SocketAPI {
	//socket 
	SOCKET Socket(int domain, int type, int protocol);
	bool   Bind(SOCKET s, const struct sockaddr * addr, uint addrlen);
	bool   Connect(SOCKET s, const struct sockaddr * addr, uint addrlen);
	bool   Listen(SOCKET s, uint backlog);
	SOCKET Accept(SOCKET s, struct sockaddr* addr, uint* addrlen);
	int    Send(SOCKET s, const void* buf, uint len, uint flags);
	int    SendTo(SOCKET s, const void* buf, int len, uint flags, const struct sockaddr* to, int tolen);
	int    Recv(SOCKET s, void* buf, uint len, uint flags);
	int    RecvFrom(SOCKET s, void* buf, int len, uint flags, struct sockaddr* from, uint* fromlen);
	bool   CloseSocket(SOCKET s);
	bool   ShutDown(SOCKET s, uint how);

	int    Select(int maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset, struct timeval* timeout);

	//socket operator
	bool   GetSockOpt(SOCKET s, int level, int optname, void* optval, uint* optlen);
	bool   SetSockOpt(SOCKET s, int level, int optname, const void* optval, uint optlen);
	uint   GetAvaliable(SOCKET s);
	bool   GetSocketNonBlocking(SOCKET s);
	bool   SetSocketNonBlocking(SOCKET s, bool on);

	std::pair<int, std::string>GetErrorMsg();	//thread safe

	bool IPConvertPToN(const char *pcszIp, void *pAddr, int family = AF_INET);
	const char *IPConvertNToP(const SOCKADDR_IN *pAddr, char *pszIp, uint len);

	bool GetSockName(SOCKET s, struct sockaddr_in& addr);
	bool GetPeerName(SOCKET s, struct sockaddr_in& addr);

};

#endif