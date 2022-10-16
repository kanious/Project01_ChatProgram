#include "ChannelManager.h"
#include "ClientInfo.h"
#include "ChannelInfo.h"

using namespace std;
ChannelManager* ChannelManager::m_pInstance = nullptr;
ChannelManager::ChannelManager()
{
	m_mapChannel.clear();
}

ChannelManager::~ChannelManager()
{
}

ChannelManager* ChannelManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ChannelManager();
	return m_pInstance;
}

void ChannelManager::DestroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

ChannelInfo* ChannelManager::GetChannelInfo(string name)
{
	ChannelMap::iterator iter = m_mapChannel.find(name);
	if (iter != m_mapChannel.end())
		return iter->second;

	return nullptr;
}

int ChannelManager::MakeChannel(string name, unsigned int requesterId)
{
	ChannelMap::iterator iter = m_mapChannel.find(name);
	if (iter != m_mapChannel.end())
		return 1;

	ChannelInfo* newChannel = new ChannelInfo;
	newChannel->masterId = requesterId;
	newChannel->name = name;
	newChannel->vecChat.clear();
	newChannel->listClient.clear();
	m_mapChannel.insert(ChannelMap::value_type(name, newChannel));

	return 0;
}

int ChannelManager::DeleteChannel(string name, unsigned int requesterId)
{
	ChannelMap::iterator iter = m_mapChannel.find(name);
	if (iter == m_mapChannel.end())
		return 1;
	
	if (nullptr == iter->second)
		return -1;

	if (iter->second->masterId != requesterId)
		return 2;

	delete iter->second;
	iter->second = nullptr;
	m_mapChannel.erase(iter);

	return 0;
}

int ChannelManager::JoinChannel(string name, ClientInfo* requester)
{
	if (nullptr == requester)
		return -1;

	ChannelMap::iterator iter = m_mapChannel.find(name);
	if (iter == m_mapChannel.end())
		return 1;

	if (nullptr == iter->second)
		return -1;

	list<string>& listClient = iter->second->listClient;
	list<string>::iterator iterClient;
	for (iterClient = listClient.begin(); iterClient != listClient.end(); ++iterClient)
	{
		if (*iterClient == requester->m_name)
			return 2;
	}

	listClient.push_back(requester->m_name);

	return 0;
}

int ChannelManager::LeaveChannel(string name, string requesterName)
{
	ChannelMap::iterator iter = m_mapChannel.find(name);
	if (iter == m_mapChannel.end())
		return 1;

	if (nullptr == iter->second)
		return -1;

	list<string>& listClient = iter->second->listClient;
	list<string>::iterator iterClient;
	for (iterClient = listClient.begin(); iterClient != listClient.end(); ++iterClient)
	{
		if (*iterClient == requesterName)
		{
			listClient.erase(iterClient);
			return 0;
		}
	}

	return 2;
}

int ChannelManager::DisconnectUserFromAllChannnel(string name, vector<ChannelInfo>& channelList)
{
	ChannelMap::iterator iter;
	for (iter = m_mapChannel.begin(); iter != m_mapChannel.end(); ++iter)
	{
		list<string>& listClient = iter->second->listClient;
		list<string>::iterator iterClient;
		for (iterClient = listClient.begin(); iterClient != listClient.end(); ++iterClient)
		{
			if (*iterClient == name)
			{
				listClient.erase(iterClient);
				channelList.push_back(*iter->second);
				break;
			}
		}
	}

	return 0;
}

int ChannelManager::ChangeNameFromAllChannel(string srcName, string desName, vector<ChannelInfo>& channelList)
{
	ChannelMap::iterator iter;
	for (iter = m_mapChannel.begin(); iter != m_mapChannel.end(); ++iter)
	{
		list<string>& listClient = iter->second->listClient;
		list<string>::iterator iterClient;
		for (iterClient = listClient.begin(); iterClient != listClient.end(); ++iterClient)
		{
			if (*iterClient == srcName)
			{
				*iterClient = desName;
				if (iter->second->name != SYSTEM_NAME)
					channelList.push_back(*iter->second);
				break;
			}
		}
	}

	return 0;
}