#ifndef _PACKET_H_
#define _PACKET_H_

#include "DLLdefine.h"
#include "PacketHeader.h"

class Buffer;
class PACKET_DLL Packet
{
public:
	PacketHeader		m_header;

public:
	explicit Packet();
	~Packet();

protected:
	virtual void Serialize(Buffer& buffer) = 0;
	virtual void Deserialize(char* dadta, unsigned int length) = 0;
};

#endif _PACKET_H_