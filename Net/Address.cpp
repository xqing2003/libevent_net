// ：
#include "Address.h"
#include "SocketAPI.h"

Address::Address() : port(-1) {
	memset(szIP, 0, IP_SIZE);
	memset(&sockAddr, 0, sizeof(sockAddr));
}
Address::Address(uint16 port, bool loopbackOnly) : Address() {
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = loopbackOnly ? htonl(INADDR_LOOPBACK) : INADDR_ANY;

	SocketAPI::IPConvertNToP(&sockAddr, szIP, IP_SIZE);
	this->port = port;
}

Address::Address(uint16 port, const char *ip) : Address() {
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	SocketAPI::IPConvertPToN(szIP, &sockAddr);

	this->port = port;
	memcpy(szIP, ip, IP_SIZE - 1);
}

Address::Address(sockaddr_in &addr) : Address() {
	sockAddr = addr;
	port = ntohs(addr.sin_port);
	SocketAPI::IPConvertNToP(&sockAddr, szIP, IP_SIZE);
}

Address::Address(bool remote, SOCKET sock) : Address() {
	if (remote) {
		SocketAPI::GetPeerName(sock, sockAddr);
	}
	else {
		SocketAPI::GetSockName(sock, sockAddr);
	}
	port = ntohs(sockAddr.sin_port);
	SocketAPI::IPConvertNToP(&sockAddr, szIP, IP_SIZE);
}