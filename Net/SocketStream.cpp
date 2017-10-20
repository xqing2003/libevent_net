
#if 1
#include "NetDefine.h"
#include "CSocket.h"


#include "SocketStream.h"

SocketStream::SocketStream( CSocket* sock, uint BufferLen, uint MaxBufferLen ) 
{
	m_pSocket = sock ;
	m_BufferLen = BufferLen ;
	m_MaxBufferLen = MaxBufferLen ;
		
	m_Head = 0 ;
	m_Tail = 0 ;
	
	m_Buffer = new char[ m_BufferLen ];
	memset( m_Buffer, 0, m_BufferLen ) ;
	m_ReadRollPos = 0;
	m_WriteRollPos = 0;
}

SocketStream::~SocketStream( ) 
{
	SAFE_DELETE_ARRAY(m_Buffer);
}

uint SocketStream::Length( )const
{
	if( m_Head <= m_Tail )
		return m_Tail-m_Head;
	
	else //if( m_Head > m_Tail ) 
		return m_BufferLen-m_Head+m_Tail ;
	
}

//返回0表示没有读到数据
uint SocketStream::Read( char* buf, uint len ) 
{
	if ( len == 0 )
		return 0 ;
		
	if ( len > Length() )
		return 0 ;
	
	if ( m_Head < m_Tail ) 
	{
		memcpy( buf, &m_Buffer[m_Head], len ) ;
	} 
	else 
	{
		uint rightLen = m_BufferLen-m_Head ;
		if( len<=rightLen ) 
		{
			memcpy( buf, &m_Buffer[m_Head], len ) ;
		} 
		else 
		{
			memcpy( buf, &m_Buffer[m_Head], rightLen ) ;
			memcpy( &buf[rightLen], m_Buffer, len-rightLen ) ;
		}
	}

	m_Head = (m_Head + len)%m_BufferLen ;
	
	return len ;
}


bool SocketStream::Peek( char* buf, uint len ) 
{			
	if( len==0 )
		return false ;
	
	if( len>Length() )
		return false ;

	if( m_Head<m_Tail ) 
	{
		memcpy( buf , &m_Buffer[m_Head] , len );

	} 
	else 
	{
		uint rightLen = m_BufferLen-m_Head ;
		if( len<=rightLen ) 
		{
			memcpy( &buf[0], &m_Buffer[m_Head], len ) ;
		} 
		else 
		{
			memcpy( &buf[0], &m_Buffer[m_Head], rightLen ) ;
			memcpy( &buf[rightLen], &m_Buffer[0], len-rightLen ) ;
		}
	}
		
	return true ;
}

bool SocketStream::Skip( uint len ) 
{
	if( len == 0 )
		return false ;

	if( len>Length( ) )
		return false ;
	
	m_Head = (m_Head+len)%m_BufferLen ;

	return true ;
}

void SocketStream::Initsize( )
{
	m_Head = 0 ;
	m_Tail = 0 ;

	SAFE_DELETE_ARRAY( m_Buffer ) ;
		
	m_Buffer = new char[DEFAULTSOCKETBUFFERSIZE] ;
//	Assert(m_Buffer) ;
		
	m_BufferLen = DEFAULTSOCKETBUFFERSIZE ;
		
	memset( m_Buffer, 0, m_BufferLen ) ;
}
int CheckReceiveError(int nReceived)
{
	if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
	if (nReceived == SOCKET_ERROR) return SOCKET_ERROR;
	if (nReceived == 0) return SOCKET_RECV_ZERO;
	return nReceived;
}
int SocketStream::Fill( ) 
{
	uint nFilled = 0 ;
	uint nReceived = 0 ;
	uint nFree = 0 ;
	int  available = m_pSocket->GetAvaliable();

	uint curLen = Length();

	if(curLen + available > m_MaxBufferLen)
	{
		return SOCKET_RECVBUFF_OVERFLOW;
	}

	if (curLen + available > Capacity())
	{
		BOOL ret = Resize((curLen + available) - Capacity());
		if(!ret)
		{
			return SOCKET_BUFF_RESIZE_FAILED;
		}
	}

	if (m_Head <= m_Tail)
	{
		nFree = m_BufferLen - m_Tail;
		if (nFree >= available)
		{
			nReceived = m_pSocket->Recv(&m_Buffer[m_Tail], available);
			int errorId = CheckReceiveError(nReceived);
			if (errorId <= 0) return errorId;
			nFilled += nReceived;
			m_Tail += nReceived;
		}
		else
		{
			nReceived = m_pSocket->Recv(&m_Buffer[m_Tail], nFree);
			int errorId = CheckReceiveError(nReceived);
			if (errorId <= 0) return errorId;
			nFilled += nReceived;

			nReceived = m_pSocket->Recv(&m_Buffer[0], available - nFree);
			errorId = CheckReceiveError(nReceived);
			if (errorId <= 0) return errorId;
			nFilled += nReceived;
			m_Tail = nReceived;
		}
	}
	else
	{
		nReceived = m_pSocket->Recv(&m_Buffer[m_Tail], available);
		int errorId = CheckReceiveError(nReceived);
		if (errorId <= 0) return errorId;
		nFilled += nReceived;
		m_Tail = nReceived;
	}


	return nFilled ;
}

bool SocketStream::Resize( uint size )
{
	if ( m_BufferLen == m_MaxBufferLen) {
		return false;
	}
	size = max(size, (int)(m_BufferLen>>1));
	uint newBufferLen = m_BufferLen + size;
	if (newBufferLen >= m_MaxBufferLen) {
		newBufferLen = m_MaxBufferLen;
	}
	uint len = Length();
	
	char * newBuffer = new CHAR[ newBufferLen ];
	if (nullptr == newBuffer)
	{
		return false;
	} 
		
	if ( m_Head < m_Tail ) 
	{
		memcpy( newBuffer , &m_Buffer[m_Head] , m_Tail - m_Head );
	} 
	else if ( m_Head > m_Tail ) 
	{
		memcpy( newBuffer , &m_Buffer[m_Head] , m_BufferLen - m_Head );
		memcpy( &newBuffer[ m_BufferLen - m_Head ] , m_Buffer , m_Tail );
	}
		
	delete [] m_Buffer ;
		
	m_Buffer = newBuffer ;
	m_BufferLen = newBufferLen ;
	m_Head = 0 ;
	m_Tail = len ;

	return true ;
}

void SocketStream::CleanUp( )
{
	m_Head = 0 ;
	m_Tail = 0 ;
}


uint SocketStream::Write(const char* buf, uint len)
{
	if (0 == len) {
		return 0;
	}
	
	uint nFree = ((m_Head <= m_Tail) ? (m_BufferLen - m_Tail + m_Head - 1) : (m_Head - m_Tail - 1));
	if (len > nFree)
	{
		if (!Resize(len - nFree))
			return 0;
	}

	if (m_Head <= m_Tail)
	{
			nFree = m_BufferLen - m_Tail;
			if (len <= nFree)
			{
				memcpy(&m_Buffer[m_Tail], buf, len);
			}
			else
			{
				memcpy(&m_Buffer[m_Tail], buf, nFree);
				memcpy(m_Buffer, &buf[nFree], len - nFree);
			}
	}
	else
	{
		memcpy(&m_Buffer[m_Tail], buf, len);
	}
	m_Tail = (m_Tail + len) % m_BufferLen;
	return len;
}

int SocketStream::Flush()
{
	uint nFlushed = 0;
	uint nSent = 0;
	uint nLeft;

	if (m_BufferLen > m_MaxBufferLen)
	{//如果单个客户端的缓存太大，则重新设置缓存，并将此客户端断开连接
		Initsize();
		return SOCKET_SENDBUFF_OVERFLOW;
	}

#if defined(WIN32)
	uint flag = MSG_DONTROUTE;
#elif defined(__LINUX__)
	uint flag = MSG_NOSIGNAL;
#endif
		if (m_Head < m_Tail)
		{
			nLeft = m_Tail - m_Head;

			while (nLeft > 0)
			{
				nSent = m_pSocket->Send(&m_Buffer[m_Head], nLeft, flag);
				if (nSent == SOCKET_ERROR_WOULDBLOCK ||
					nSent == 0) {
					return 0;
				}
				else if (nSent < 0)
				{
					return nSent;
				}

				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}

			//			Assert( nLeft == 0 );

		}
		else if (m_Head > m_Tail)
		{
			nLeft = m_BufferLen - m_Head;

			while (nLeft > 0)
			{
				nSent = m_pSocket->Send(&m_Buffer[m_Head], nLeft, flag);
				if (nSent == SOCKET_ERROR_WOULDBLOCK ||
					nSent == 0) {
					return 0;
				}
				else if (nSent < 0)
				{
					return nSent;
				}

				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}

			//			Assert( m_Head == m_BufferLen );

			m_Head = 0;

			nLeft = m_Tail;

			while (nLeft > 0)
			{
				nSent = m_pSocket->Send(&m_Buffer[m_Head], nLeft, flag);
				if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
				if (nSent == 0) return 0;
				if (nSent < 0)
				{
					return nSent;
				}

				nFlushed += nSent;
				nLeft -= nSent;
				m_Head += nSent;
			}
		}

	m_Head = m_Tail = 0;
	return nFlushed;
}


uint SocketStream::Write(int data) {
	int value = htonl(data);
	return Write((char *)&value, sizeof(int));
}

uint SocketStream::Read(int &data) {
	char szBuff[MAX_PATH] = { 0 };
	if (0 == Read(szBuff, sizeof(int))) {
		return 0;
	}
	int value = *(int *)szBuff;
	data = ntohl(value);
	return sizeof(int);
}

uint SocketStream::Read(long &data) {
	char szBuff[MAX_PATH] = { 0 };
	if (0 == Read(szBuff, sizeof(long))) {
		return 0;
	}
	int value = *(long *)szBuff;
	data = ntohll(value);
	return sizeof(long);
}

uint SocketStream::Peek(int &data) {
	char szBuff[MAX_PATH] = { 0 };
	if (false == Peek(szBuff, sizeof(int))) {
		return 0;
	}
	int value = *(int *)szBuff;
	data = ntohl(value);
	return sizeof(int);
}

#endif