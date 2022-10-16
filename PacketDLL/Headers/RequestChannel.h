#ifndef _REQUESTCHANNELMAKE_H_
#define _REQUESTCHANNELMAKE_H_

#include "Packet.h"

class PACKET_DLL pRequestChannel : public Packet
{
public:
	unsigned int			clientId;
	std::string				channelName;

public:
	explicit pRequestChannel();
	virtual ~pRequestChannel();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_REQUESTCHANNELMAKE_H_