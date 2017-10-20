#include "BaseTypeDef.h"
#include "NetDefine.h"
#include "SocketAPI.h"
#include "CSocket.h"

CSocket::CSocket() {
	m_nSocketID = INVALID_SOCKET;
	memset(&m_oSockAddr, 0, sizeof(m_oSockAddr));
	memset(m_szIP, 0, sizeof(char) * IP_SIZE);
	m_nPort = -1;
	m_bNoBlocking = false;
}

CSocket::CSocket(const char *psczIP, uint port) : CSocket() {
	strncpy(m_szIP, psczIP, IP_SIZE);
	m_nPort = port;
}

CSocket::CSocket(SOCKET s, const struct sockaddr_in *addr) {
	m_nSocketID = s;
	memcpy(&m_oSockAddr, addr, sizeof(*addr));
	m_nPort = ntohs(m_oSockAddr.sin_port);
	SocketAPI::IPConvertNToP(&m_oSockAddr, m_szIP, IP_SIZE);
	m_bNoBlocking = false;
}

CSocket::CSocket(const CSocket &rhs) {
	*this = rhs;
}

CSocket &CSocket::operator=(const CSocket &rhs) {
	if (this == &rhs) {
		return *this;
	}
	m_nSocketID = rhs.m_nSocketID;
	memcpy(&m_oSockAddr, &rhs.m_oSockAddr, sizeof(m_oSockAddr));
	memcpy(m_szIP, rhs.m_szIP, IP_SIZE);
	m_nPort = rhs.m_nPort;
	m_bNoBlocking = rhs.m_bNoBlocking;
	return *this;
}

CSocket::~CSocket() {
	;
}

bool CSocket::Create() {
	m_nSocketID = SocketAPI::Socket(AF_INET, SOCK_STREAM, 0);
	memset(&m_oSockAddr, 0, sizeof(m_oSockAddr));
	m_oSockAddr.sin_family = AF_INET;
	return IsValidSocket();
}

void CSocket::Close() {
	if (IsValidSocket() && false == IsSocketError()) {
		SocketAPI::CloseSocket(m_nSocketID);
	}
	m_nSocketID = INVALID_SOCKET;
	memset(&m_oSockAddr, 0, sizeof(m_oSockAddr));
	memset(m_szIP, 0, IP_SIZE);
	m_nPort = 0;
}

bool CSocket::Connect(const char *psczIP /* = NULL */, uint port /* = -1 */) {
	if (nullptr != psczIP) {
		strncpy(m_szIP, psczIP, IP_SIZE);
	}
	if (-1 != port) {
		m_nPort = port;
	}
	m_oSockAddr.sin_port = htons(m_nPort);
	//m_oSockAddr.sin_addr.s_addr = inet_addr(m_szIP);
	SocketAPI::IPConvertPToN(m_szIP, &m_oSockAddr.sin_addr);
	return SocketAPI::Connect(m_nSocketID, (const struct sockaddr *)&m_oSockAddr, sizeof(m_oSockAddr));
}

bool CSocket::Reconnect(const char *psczIP, uint port) {
	Close();
	strncpy(m_szIP, psczIP, IP_SIZE);
	m_nPort = port;

	Create();

	return Connect(m_szIP, m_nPort);
}

bool CSocket::Bind(uint port) {
	m_nPort = port;
	m_oSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_oSockAddr.sin_port = htons(m_nPort);
	return SocketAPI::Bind(m_nSocketID, (const struct sockaddr *)&m_oSockAddr, sizeof(m_oSockAddr));
}

bool CSocket::Listen(int backlog) {
	return SocketAPI::Listen(m_nSocketID, backlog);
}

SOCKET CSocket::Accept(struct sockaddr *addr, uint *addrLen) {
	SOCKET client = SocketAPI::Accept(m_nSocketID, addr, addrLen);
	return client;
}

#if 0
CSocket *CSocket::Accept() {
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	uint size = sizeof(addr);
	SOCKET client = Accept((struct sockaddr *)&addr, &size);
	if (INVALID_SOCKET == client) {
		return nullptr;
	}

	CSocket *pSocket = new CSocket(client, &addr);
	return pSocket;
}
#endif

bool CSocket::Accept(CSocket &rhs) {
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	uint size = sizeof(addr);
	SOCKET client = Accept((struct sockaddr *)&addr, &size);
	if (INVALID_SOCKET == client) {
		return false;
	}
	rhs = CSocket(client, &addr);
	return true;
}

int CSocket::Send(const void *buf, int len, uint flags /* = 0 */) {
	return SocketAPI::Send(m_nSocketID, buf, len, flags);
}

int CSocket::Recv(void* buf, uint len, uint flags /* = 0 */) {
	return SocketAPI::Recv(m_nSocketID, buf, len, flags);
}

bool CSocket::IsValidSocket() const {
	return INVALID_SOCKET != m_nSocketID;
}

bool CSocket::IsSocketError() const {
	int error;
	uint len = sizeof(error);

	int Result = SocketAPI::GetSockOpt(m_nSocketID, SOL_SOCKET, SO_ERROR, &error, &len);

	if (Result == 0)
		return false;
	else
		return true;
}

SOCKET CSocket::GetSocket() const {
	return m_nSocketID;
}

uint CSocket::GetSendBuffSize() const {
	uint sendBuffSize = 0;
	uint size = sizeof(sendBuffSize);
	SocketAPI::GetSockOpt(m_nSocketID, SOL_SOCKET, SO_SNDBUF, &sendBuffSize, &size);
	return sendBuffSize;
}

bool CSocket::SetSendBuffSize(uint size) {
	return SocketAPI::SetSockOpt(m_nSocketID, SOL_SOCKET, SO_SNDBUF, &size, sizeof(uint));
}

uint CSocket::GetRecvBuffSize() const {
	uint recvBuffSize = 0;
	uint size = sizeof(recvBuffSize);
	SocketAPI::GetSockOpt(m_nSocketID, SOL_SOCKET, SO_RCVBUF, &recvBuffSize, &size);
	return recvBuffSize;

}

bool CSocket::SetRecvBuffSize(uint size) {
	return SocketAPI::SetSockOpt(m_nSocketID, SOL_SOCKET, SO_RCVBUF, &size, sizeof(uint));
}

bool CSocket::IsNonBlocking() const {
	return m_bNoBlocking;
}

bool CSocket::SetNonBlocking(bool on /* = TRUE */) {
	bool ret = SocketAPI::SetSocketNonBlocking(m_nSocketID, on);
	if (ret) {
		m_bNoBlocking = on;
	}
	return ret;
}

uint CSocket::GetSockErrorID() const {
	return SocketAPI::GetErrorMsg().first;
}

std::string CSocket::GetSockErrorMsg() const {
	return SocketAPI::GetErrorMsg().second;
}

uint CSocket::GetAvaliable() const {
	return SocketAPI::GetAvaliable(m_nSocketID);
}

bool CSocket::GetSockOpt(int optname, void* optval, uint* optlen, int level /* = SOL_SOCKET */) {
	return SocketAPI::GetSockOpt(m_nSocketID, level, optname, optval, optlen);
}

bool CSocket::SetSockOpt(int optname, const void* optval, uint optlen, int level /* = SOL_SOCKET */) {
	return SocketAPI::SetSockOpt(m_nSocketID, level, optname, optval, optlen);
}

const char * CSocket::GetRemoteIP() const {
	return m_szIP;
}