#ifndef _CHANNELMANAGER_H_
#define _CHANNELMANAGER_H_

#include "Define.h"

class ClientInfo;
class ChannelInfo;
class ChannelManager
{
private:
	static ChannelManager*		m_pInstance;

private:
	typedef std::unordered_map<std::string, ChannelInfo*> ChannelMap;
	ChannelMap					m_mapChannel;

public:
	explicit ChannelManager();
	~ChannelManager();
	static ChannelManager* GetInstance();
	void DestroyInstance();
public:
	ChannelMap* GetChannelMap() { return &m_mapChannel; }

public:
	ChannelInfo* GetChannelInfo(std::string name);
	int MakeChannel(std::string name, unsigned int requesterId);
	int DeleteChannel(std::string name, unsigned int requesterId);
	int JoinChannel(std::string name, ClientInfo* requester);
	int LeaveChannel(std::string name, std::string requesterName);
	int DisconnectUserFromAllChannnel(std::string name, std::vector<ChannelInfo>& channelList);
	int ChangeNameFromAllChannel(std::string srcName, std::string desName, std::vector<ChannelInfo>& channelList);
};

#endif //_CHANNELMANAGER_H_