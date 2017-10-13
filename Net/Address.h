#pragma once
// ：
#include "NetDefine.h"
class Address
{
public:
	Address(uint16 port, bool loopbackOnly);
	Address(uint16 port, const char *ip);
	Address(sockaddr_in &addr);
	Address(bool remote, SOCKET sock);
protected:
	Address();
public:
	const struct sockaddr_in& get_sockaddr_in() const { return sockAddr; }
	//// get ip address like "123.123.123.123"
	const char*	get_ip(void) const { return szIP; }
	uint16	get_port(void) const { return port; }
private:
	char szIP[IP_SIZE];
	uint16 port;
	struct sockaddr_in sockAddr;

};