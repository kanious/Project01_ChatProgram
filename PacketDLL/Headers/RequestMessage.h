#ifndef _REQUESTMESSAGE_H_
#define _REQUESTMESSAGE_H_

#include "Packet.h"
class PACKET_DLL pRequestMessage : public Packet
{
public:
	unsigned int		clientId;
	std::string			channelName;
	std::string			message;

public:
	explicit pRequestMessage();
	virtual ~pRequestMessage();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_REQUESTMESSAGE_H_