#ifndef _RESPONSECONNECT_H_
#define _RESPONSECONNECT_H_

#include "Packet.h"
#include "ChannelInfo.h"

class PACKET_DLL pResponseConnect : public Packet
{
public:
	unsigned int				clientId;
	std::string					clientName;
	std::vector<std::string>	vecAllChannels;
	std::vector<ChannelInfo>	vecJoinedChannels;

public:
	explicit pResponseConnect();
	virtual ~pResponseConnect();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_RESPONSECONNECT_H_