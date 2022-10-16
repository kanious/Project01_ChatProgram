#ifndef _CHANNELINFO_H_
#define _CHANNELINFO_H_

#include "DLLdefine.h"

class PACKET_DLL ChannelInfo
{
public:
	unsigned int				masterId;
	std::string					name;
	std::list<std::string>		listClient;
	std::vector<std::string>	vecChat;

public:
	explicit ChannelInfo() {}
	~ChannelInfo() {}
};

#endif //_CHANNELINFO_H_