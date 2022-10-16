#ifndef _REQUESTDISCONNECT_H_
#define _REQUESTDISCONNECT_H_

#include "Packet.h"
class PACKET_DLL pRequestDisconnect : public Packet
{
public:
	unsigned int		clientId;

public:
	explicit pRequestDisconnect();
	virtual ~pRequestDisconnect();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_REQUESTDISCONNECT_H_