// libeventTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "event2/event.h"
#include <iostream>
#include <WinSock2.h>
#include <cstdio>
#include "Net/NetManager.h"
#include "Net/CSocket.h"
#include "Net/Address.h"
#pragma comment(lib, "ws2_32")
using namespace std;
#if 0

void UpdateSocketError() {

#if defined(WIN32)

#ifdef _UNICODE
	wchar_t szBuff[1024] = { 0 };
	typedef LPWSTR STRTYPE;
#else
	char szBuff[1024] = { 0 };
	typedef LPSTR STRTYPE;
#endif
	DWORD eCode = ::WSAGetLastError();
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		eCode,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(STRTYPE)szBuff,
		0, nullptr);

	string errorMsg;
#ifdef _UNICODE
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate));

	int requiredNum = wcrtomb(nullptr, *szBuff, &mbstate);
	char *pBuff = new char[requiredNum + 1];
	memset(pBuff, 0, sizeof(char) * (requiredNum + 1));
	wcrtomb(pBuff, *szBuff, &mbstate);
	errorMsg = pBuff;
	delete[]pBuff;
	pBuff = nullptr;
#else
	errorMsg = szBuff;
#endif


#elif defined(__LINUX__)

#endif

}

SOCKET createListenSocket(int port) {
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock) {
		cout << "create socket error" << endl;
		return sock;
	}
	int reuseAddr = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof(int));
	if (SOCKET_ERROR == ret) {
		cout << "set reuse addr error" << endl;
	}
	u_long nonblock = 1;
	ret = ioctlsocket(sock, FIONBIO, &nonblock);
	if (ret < 0) {
		cout << "set non block error" << endl;
	}
	ret = bind(sock, (sockaddr *)&sin, sizeof(sin));
	if (SOCKET_ERROR == ret) {
		UpdateSocketError();
		cout << "bind error" << endl;
	}

	ret = listen(sock, 10);
	if (SOCKET_ERROR == ret) {
		cout << "listen error" << endl;
	}
	return sock;
}
struct event_base *base;

void acceptNewConnection(SOCKET sock) {
	//event *ev = event_new()
}

void listenCallback(evutil_socket_t sock, short events, void *data) {
	if (events & EV_READ) {
		sockaddr_in sockAddr;
		int len = sizeof(sockAddr);
		SOCKET s = accept(sock, (sockaddr *)&sockAddr, &len);
		
		acceptNewConnection(s);
		cout << "accept new connection ip = " << inet_ntoa(sockAddr.sin_addr) << " port "<< sockAddr.sin_port << endl;
	}

}


#endif

int _tmain(int argc, _TCHAR* argv[])
{
	struct event *ev;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
	Address addr(111, false);
	NetManager manager;
	return 0;
}

