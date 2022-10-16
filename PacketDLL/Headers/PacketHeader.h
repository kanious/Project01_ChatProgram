#ifndef _PACKETHEADER_H_
#define _PACKETHEADER_H_

#include "DLLdefine.h"

class PACKET_DLL PacketHeader
{
public:
	unsigned int length;
	unsigned int messageId;

public:
	explicit PacketHeader();
	~PacketHeader();

public:
	void Deserialize(char*);

};

#endif //_PACKETHEADER_H_