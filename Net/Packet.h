#pragma once
// ：

#include "BaseTypeDef.h"

class SocketStream;
struct PacketHead 
{
	int msgId;
	int msgLen;
};

class Packet
{
public:
	Packet() {}
	virtual ~Packet() {}
public:
	virtual int getPacketId() { return -1; }
	virtual int getPacketLen() { return 0; }
	virtual bool read(SocketStream &stream, int len){ return false; }
	virtual void write(SocketStream &stream) {}
};