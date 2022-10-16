#ifndef _RESPONSEERROR_H_
#define _RESPONSEERROR_H_

#include "Packet.h"

class PACKET_DLL pResponseError : public Packet
{
public:
	unsigned int			clientId;
	std::string				channelName;
	std::string				clientName;
	std::string				errorMsg;

public:
	explicit pResponseError();
	virtual ~pResponseError();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_RESPONSEERROR_H_