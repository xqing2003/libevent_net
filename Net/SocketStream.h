//
//文件名称：	SocketInputStream.h
//功能描述：	消息数据的接收缓存，提供数据的接收和格式化读取功能
//				
//
//

#ifndef __SOCKETSTREAM_H__
#define __SOCKETSTREAM_H__


class CSocket;
//初始化的缓存长度
#define DEFAULTSOCKETBUFFERSIZE 64*1024
//最大可以允许的缓存长度，如果超过此数值，则断开连接
#define DISCONNECTSOCKETINPUTSIZE 96*1024
#define DISCONNECTSOCKETOUTPUTSIZE 512*1024
class SocketStream
{
public :
	SocketStream( CSocket* sock, 
		uint BufferSize = DEFAULTSOCKETBUFFERSIZE,
		uint MaxBufferSize = DISCONNECTSOCKETINPUTSIZE ) ;
	virtual ~SocketStream( ) ;

public :
	void						StartTransaction(bool read = true) { 
		if (read) {
			m_ReadRollPos = m_Head;
		}
		else {
			m_WriteRollPos = m_Tail;
		}
	}
	void						Commit(bool read = true) { 
		if (read) {
			m_ReadRollPos = -1;
		}
		else {
			m_WriteRollPos = -1;
		}
	}
	void						RollBack(bool read = true) {
		if (read) {
			m_Head = m_ReadRollPos;
		}
		else {
			m_Tail = m_WriteRollPos;
		}
	}
	uint						Write(const char* buf, uint len);
	uint						Read( char* buf, uint len ) ;
	
	uint						Write(int data);
	uint						Read(int &data);
	uint						Read(long &data);
	uint						Peek(int &data);

	bool						Peek( char* buf, uint len ) ;
	bool						Skip( uint len ) ;

	int							Flush();
	int							Fill( ) ;

	void						Initsize( ) ;
	bool						Resize( uint size ) ;
	
	uint						Capacity( )const { return m_BufferLen; }
	
	uint						Length( )const ;

	bool						IsEmpty( )const { return m_Head==m_Tail; }

	void						CleanUp( ) ;

	uint						GetHead(){ return m_Head; }
	uint						GetTail(){ return m_Tail; }
	//uint						GetBuffLen(){ return m_BufferLen; }
	char*						GetBuff(){ return m_Buffer; }
	char*						GetBuff()const { return m_Buffer; }
	char*						GetTail()const { return &(m_Buffer[m_Tail]); }
	uint						GetFreeLength() const { return m_BufferLen - Length() - 1; }
private :
	CSocket*	m_pSocket;	
	char*		m_Buffer;
	
	uint		m_BufferLen;
	uint		m_MaxBufferLen;
	
	uint		m_Head;
	uint		m_Tail;
	uint		m_ReadRollPos;
	uint		m_WriteRollPos;
};
#endif
