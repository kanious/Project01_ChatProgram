#ifndef _RESPONSEMESSAGE_H_
#define _RESPONSEMESSAGE_H_

#include "Packet.h"
class PACKET_DLL pResponseMessage : public Packet
{
public:
	unsigned int		clientId;
	std::string			channelName;
	std::string			clientName;
	std::string			message;

public:
	explicit pResponseMessage();
	virtual ~pResponseMessage();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_RESPONSEMESSAGE_H_