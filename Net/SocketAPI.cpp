#include "SocketAPI.h"

#if defined(WIN32)
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32")
#endif

std::map<int, std::pair<int, std::string> > g_oErrorMsg;

#if defined(WIN32)
class WinSockInit {
public:
	WinSockInit() {
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		::WSAStartup(wVersionRequested, &wsaData);
	}
	~WinSockInit() {
		::WSACleanup();
	}
};
static WinSockInit s_oInit;
#endif

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

	std::string errorMsg;
#ifdef _UNICODE
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate));

	int requiredNum = wcrtomb(nullptr, *szBuff, &mbstate);
	char *pBuff = new char[requiredNum + 1];
	memset(pBuff, 0, sizeof(char) * (requiredNum + 1));
	wcrtomb(pBuff, *szBuff, &mbstate);
	errorMsg = pBuff;
	SAFE_DELETE(pBuff);
#else
	errorMsg = szBuff;
#endif

	DWORD threadId = ::GetCurrentThreadId();
	g_oErrorMsg[threadId] = make_pair(eCode, errorMsg);

#elif defined(__LINUX__)

#endif

}

namespace SocketAPI {

	SOCKET Socket(int domain, int type, int protocol) {
		SOCKET s = ::socket(domain, type, protocol);
		if (INVALID_SOCKET == s) {
			UpdateSocketError();
		}
		return s;
	}
	bool   Bind(SOCKET s, const struct sockaddr * addr, uint addrlen) {
		int ret = bind(s, addr, addrlen);
		if (SOCKET_ERROR == ret) {
			UpdateSocketError();
		}
		return SOCKET_ERROR != ret;
	}

	bool   Connect(SOCKET s, const struct sockaddr * addr, uint addrlen) {
		int ret = connect(s, addr, addrlen);
		if (SOCKET_ERROR == ret) {
			UpdateSocketError();
		}

		return SOCKET_ERROR != ret;
	}
	bool   Listen(SOCKET s, uint backlog) {
		int ret = listen(s, backlog);
		if (SOCKET_ERROR == ret) {
			UpdateSocketError();
		}
		return SOCKET_ERROR != ret;
	}
	SOCKET Accept(SOCKET s, struct sockaddr* addr, uint* addrlen) {
#if defined(WIN32)
		SOCKET client = accept(s, addr, (int *)addrlen);
#elif defined(__LINUX__)
		SOCKET client = accept(s, addr, addrlen);
#endif
		if (INVALID_SOCKET == client) {
			UpdateSocketError();
		}
		return client;
	}
	int   Send(SOCKET s, const void* buf, uint len, uint flags) {
		int nSend;
#if  defined(WIN32)
		nSend = send(s, (const char *)buf, len, flags);
#elif defined(__LINUX__)
		nSend = send(s, buf, len, flags);
#endif

		if (SOCKET_ERROR == nSend) {
			UpdateSocketError();
		}
		return nSend;
	}

	int   Recv(SOCKET s, void* buf, uint len, uint flags) {
#if defined(WIN32)
		int nRecv = recv(s, (char*)buf, len, flags);
#elif defined(__LINUX__)
		int nRecv = recv(s, buf, len, flags);
#endif
		if (SOCKET_ERROR == nRecv) {
			UpdateSocketError();
		}
		return nRecv;
	}

	int   SendTo(SOCKET s, const void* buf, int len, uint flags, const struct sockaddr* to, int tolen) {
#if defined(WIN32)
		int nSend = sendto(s, (const char *)buf, len, flags, to, tolen);
#elif defined(__LINUX__)
		int nSend = sendto(s,buf,len,flags,to,tolen);
#endif
		if (SOCKET_ERROR == nSend) {
			UpdateSocketError();
		}
		return nSend;
	}
	int   RecvFrom(SOCKET s, void* buf, int len, uint flags, struct sockaddr* from, uint* fromlen) {
#if defined(WIN32)
		int nRecv = recvfrom(s, (char*)buf, len, flags, from, (int*)fromlen);
#elif defined(__LINUX__)
		int nRecv = recvfrom(s,buf,len,flags,from,fromlen);
#endif
		if (SOCKET_ERROR == nRecv) {
			UpdateSocketError();
		}

		return nRecv;
	}

	bool   CloseSocket(SOCKET s) {
#if defined(WIN32)
		if (SOCKET_ERROR == closesocket(s)) {
			UpdateSocketError();
			return false;
		}
#elif defined(__LINUX__)
		close(s);
#endif
		return true;
	}
	bool   ShutDown(SOCKET s, uint how) {
		if (shutdown(s, how) < 0) {
			UpdateSocketError();
			return false;
		}
		return true;
	}

	int Select(int maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset, struct timeval* timeout) {
		int result = select(maxfdp1, readset, writeset, exceptset, timeout);
		if (SOCKET_ERROR == result) {
			UpdateSocketError();
		}
		return result;
	}

	bool   GetSockOpt(SOCKET s, int level, int optname, void* optval, uint* optlen) {
#if defined(WIN32)
		if (SOCKET_ERROR == getsockopt(s, level, optname, (char*)optval, (int*)optlen)) {
			UpdateSocketError();
			return false;
		}
#elif defined(__LINUX__)
		if (SOCKET_ERROR == getsockopt(s , level , optname , optval , optlen)) {
			return false;
		}
#endif
		return true;
	}
	bool   SetSockOpt(SOCKET s, int level, int optname, const void* optval, uint optlen) {
#if defined(WIN32)
		if (SOCKET_ERROR == setsockopt(s, level, optname, (char*)optval, optlen)) {
			UpdateSocketError();
			return false;
		}
#elif defined(__LINUX__)
		if (SOCKET_ERROR == setsockopt( s , level , optname , optval , optlen )) {
			return false;
		}
#endif
		return true;
	}

	uint   GetAvaliable(SOCKET s) {
#if defined(WIN32)
		ULONG avaliable = 0;
		::ioctlsocket(s, FIONREAD, &avaliable);
		return avaliable;
#elif defined(__LINUX__)
		uint arg = 0;
		ioctl(fd, FIONREAD, &arg);
		return arg;
#endif
	}
	bool   GetSocketNonBlocking(SOCKET s) {
#if defined(WIN32)
		return false;
#elif defined(__LINUX__)
		int flags = fcntl ( s , F_GETFL , 0);
		return flags | O_NONBLOCK;
#endif
	}
	bool   SetSocketNonBlocking(SOCKET s, bool on) {
#if defined(WIN32)
		ULONG argp = (on == true) ? 1 : 0;
		return ioctlsocket(s, FIONBIO, &argp);
#elif defined(__LINUX__)
		int flags = FileAPI::fcntl_ex(s , F_GETFL , 0);
		if (on)
			// set nonblocking
			flags |= O_NONBLOCK;
		else
			// set blocking
			flags &= ~O_NONBLOCK;
		fcntl(s, F_SETFL, flags);
		return true;
#endif
	}

	std::pair<int, std::string>GetErrorMsg() {
#if defined(WIN32)
		DWORD threadId = GetCurrentThreadId();
		return g_oErrorMsg[threadId];
#elif defined(__LINUX__)
		//TODO
		return make_pair(-1, "");
#endif
	}

	bool IPConvertPToN(const char *pcszIp, void *pAddr, int family/* = AF_INET*/) {
#if defined(WIN32)
		inet_pton(family, pcszIp, pAddr);
#elif defined(__LINUX__) 

#endif
		return true;
	}
	const char *IPConvertNToP(const SOCKADDR_IN *pAddr, char *pszIp, uint len) {
#if defined(WIN32)
		inet_ntop(AF_INET, (PVOID)(&pAddr->sin_addr), pszIp, len);
		//const CHAR *pszTemp = inet_ntoa(pAddr->sin_addr);
		//strncpy(pszIp, pszTemp, len);
		return pszIp;
#elif defined(__LINUX__) 
		if (AF_INET == pAddr->sin_family) {
			assert(len >= INET_ADDRSTRLEN);
			inet_ntop(AF_INET, &pAddr->sin_addr, pszIp, len);
	}
		else if (AF_INET6 == pAddr->sin_family) {
			assert(len >= INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &pAddr->sin_addr, pszIp, len);
		}
		return pszIp;
#endif
	}

	//-------------------------------------------------------------------------------------
	bool GetSockName(SOCKET s, struct sockaddr_in& addr)
	{
		socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
		memset(&addr, 0, sizeof addr);

#ifdef WIN32
		if (::getsockname(s, (struct sockaddr*)(&addr), &addrlen) == SOCKET_ERROR)
#else
		if (::getsockname(s, (struct sockaddr*)(&addr), &addrlen) < 0)
#endif
		{
			return false;
		}
		return true;
	}

	//-------------------------------------------------------------------------------------
	bool GetPeerName(SOCKET s, struct sockaddr_in& addr)
	{
		socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
		memset(&addr, 0, sizeof addr);

#ifdef WIN32
		if (::getpeername(s, (struct sockaddr*)(&addr), &addrlen) == SOCKET_ERROR)
#else
		if (::getpeername(s, (struct sockaddr*)(&addr), &addrlen) < 0)
#endif
		{
			return false;
		}
		return true;
	}

}