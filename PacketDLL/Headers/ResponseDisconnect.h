#ifndef _RESPONSEDISCONNECT_H_
#define _RESPONSEDISCONNECT_H_

#include "Packet.h"
class PACKET_DLL pResponseDisconnect : public Packet
{
public:
	unsigned int		clientId;

public:
	explicit pResponseDisconnect();
	virtual ~pResponseDisconnect();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_RESPONSEDISCONNECT_H_