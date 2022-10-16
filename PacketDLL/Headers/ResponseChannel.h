#ifndef _RESPONSECHANNEL_H_
#define _RESPONSECHANNEL_H_

#include "Packet.h"
#include "ChannelInfo.h"

class PACKET_DLL pResponseChannel : public Packet
{
public:
	std::vector<std::string>	vecAllChannels;
	std::vector<ChannelInfo>	vecJoinedChannels;
	std::string					channelName;
	std::string					clientName;

public:
	explicit pResponseChannel();
	virtual ~pResponseChannel();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_RESPONSECHANNEL_H_