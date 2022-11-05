#include "Buffer.h"
#include "ServerSendManager.h"
#include "Server.h"
#include "DLLPacketIncludes.h"
#include "ChannelManager.h"

using namespace std;
ServerSendManager* ServerSendManager::m_pInstance = nullptr;
ServerSendManager::ServerSendManager()
{
	m_pAuthClient = nullptr;
}

ServerSendManager::~ServerSendManager()
{
}

ServerSendManager* ServerSendManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ServerSendManager();
	return m_pInstance;
}

void ServerSendManager::DestroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ServerSendManager::SendPacket(ClientInfo* pClient, char* data, int length)
{
	int sendResult = send(pClient->m_socket, data, length, 0);
	if (sendResult == SOCKET_ERROR)
		pClient->m_bConnected = false;
}

void ServerSendManager::AcceptClient(ClientInfo* pClient)
{
	Buffer buffer(0);
	pResponseConnect response;
	response.m_header.messageId = CONNECT;
	response.clientId = pClient->m_id;
	response.clientName = pClient->m_name;
	unordered_map<string, ChannelInfo*>* pChannelMap = ChannelManager::GetInstance()->GetChannelMap();
	if (nullptr != pChannelMap)
	{
		unordered_map<string, ChannelInfo*>::iterator iter;
		for (iter = pChannelMap->begin(); iter != pChannelMap->end(); ++iter)
		{
			response.vecAllChannels.push_back(iter->second->name);
			if (iter->second->masterId == SYSTEM_ID)
			{
				ChannelInfo channel;
				channel.masterId = iter->second->masterId;
				channel.name = iter->second->name;
				channel.listClient.push_back(pClient->m_name);
				response.vecJoinedChannels.push_back(channel);
			}
		}
	}
	response.Serialize(buffer);

	SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::SendErrorMessageToClient(ClientInfo* pClient, string errMsg)
{
	Buffer buffer(0);
	pResponseError response;
	response.m_header.messageId = ERROR_MSG;
	response.clientId = SYSTEM_ID;
	response.channelName = SYSTEM_CHANNEL;
	response.clientName = SYSTEM_NAME;
	response.errorMsg = errMsg;
	response.Serialize(buffer);

	SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::SendMessageToClient(ClientInfo* pClient, char* data, unsigned int length)
{
	if (nullptr == m_pAuthClient)
		return;

	pRequestMessage packet;
	packet.Deserialize(data, length);

	Buffer buffer(0);
	pResponseMessage response;
	response.m_header.messageId = SEND_MSG;
	response.clientId = packet.clientId;
	response.channelName = packet.channelName;
	response.clientName = m_pAuthClient->GetClientName(packet.clientId);
	response.message = packet.message;
	response.Serialize(buffer);

	SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::BroadcastMessageToClients(char* data, unsigned int length)
{
	if (nullptr == m_pAuthClient)
		return;

	pRequestMessage packet;
	packet.Deserialize(data, length);

	Buffer buffer(0);
	pResponseMessage response;
	response.m_header.messageId = SEND_MSG;
	response.clientId = packet.clientId;
	response.channelName = packet.channelName;
	response.clientName = m_pAuthClient->GetClientName(packet.clientId);
	response.message = packet.message;
	response.Serialize(buffer);

	list<ClientInfo*>::iterator iter;
	for (iter = m_pAuthClient->m_listAuthClients.begin(); iter != m_pAuthClient->m_listAuthClients.end(); ++iter)
	{
		ClientInfo* client = *iter;
		if (!client->m_bConnected)
			continue;
		SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
	}
}

void ServerSendManager::SendSystemMessageToClient(ClientInfo* pClient, string message)
{
	if (nullptr == m_pAuthClient)
		return;

	Buffer buffer(0);
	pResponseMessage response;
	response.m_header.messageId = SEND_MSG;
	response.clientId = SYSTEM_ID;
	response.channelName = SYSTEM_CHANNEL;
	response.clientName = SYSTEM_NAME;
	response.message = message;
	response.Serialize(buffer);

	SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::BroadcastChannelMakeMessage(string channelName)
{
	Buffer buffer(0);
	ChannelInfo* pInfo = ChannelManager::GetInstance()->GetChannelInfo(channelName);
	pResponseChannel response;
	response.m_header.messageId = MAKE_CHANNEL;
	response.vecAllChannels.push_back(pInfo->name);
	response.Serialize(buffer);

	list<ClientInfo*>::iterator iter;
	for (iter = m_pAuthClient->m_listAuthClients.begin(); iter != m_pAuthClient->m_listAuthClients.end(); ++iter)
	{
		ClientInfo* client = *iter;
		if (!client->m_bConnected)
			continue;

		bool isMaster = false;
		list<string>::iterator iterlist;
		for (iterlist = pInfo->listClient.begin(); iterlist != pInfo->listClient.end(); ++iterlist)
		{
			if (*iterlist == client->m_name)
			{
				isMaster = true;
				break;
			}
		}

		if (isMaster)
		{
			Buffer bufferForMaster(0);
			pResponseChannel responseForMaster;
			responseForMaster.m_header.messageId = MAKE_CHANNEL;
			responseForMaster.vecAllChannels.push_back(pInfo->name);
			responseForMaster.vecJoinedChannels.push_back(*pInfo);
			responseForMaster.Serialize(bufferForMaster);
			SendPacket(client, (char*)&(bufferForMaster.m_data[0]), responseForMaster.m_header.length);
		}
		else
		{
			SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
		}
	}
}

void ServerSendManager::BroadcastChannelDeleteMessage(string channelName)
{
	Buffer buffer(0);
	pResponseChannel response;
	response.m_header.messageId = DELETE_CHANNEL;
	response.channelName = channelName;
	response.Serialize(buffer);

	list<ClientInfo*>::iterator iter;
	for (iter = m_pAuthClient->m_listAuthClients.begin(); iter != m_pAuthClient->m_listAuthClients.end(); ++iter)
	{
		ClientInfo* client = *iter;
		if (!client->m_bConnected)
			continue;
		SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
	}
}

void ServerSendManager::BroadcastChannelJoinMessage(std::string channelName)
{
	Buffer buffer(0);
	ChannelInfo* pInfo = ChannelManager::GetInstance()->GetChannelInfo(channelName);
	pResponseChannel response;
	response.m_header.messageId = JOIN_CHANNEL;
	response.vecJoinedChannels.push_back(*pInfo);
	response.Serialize(buffer);

	list<string>::iterator iter;
	for (iter = pInfo->listClient.begin(); iter != pInfo->listClient.end(); ++iter)
	{
		ClientInfo* client = m_pAuthClient->GetClient(*iter);
		if (nullptr != client)
			SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
	}
}

void ServerSendManager::BroadcastChannelLeaveMessage(ClientInfo* pClient, std::string channelName)
{
	Buffer buffer(0);
	ChannelInfo* pInfo = ChannelManager::GetInstance()->GetChannelInfo(channelName);
	pResponseChannel response;
	response.m_header.messageId = LEAVE_CHANNEL;
	response.vecJoinedChannels.push_back(*pInfo);
	response.Serialize(buffer);

	list<string>::iterator iter;
	for (iter = pInfo->listClient.begin(); iter != pInfo->listClient.end(); ++iter)
	{
		ClientInfo* client = m_pAuthClient->GetClient(*iter);
		if (nullptr != client)
			SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
	}

	if (nullptr != pClient)
		SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::SendDisconnectResponse(ClientInfo* pClient)
{
	Buffer buffer(0);
	pResponseDisconnect response;
	response.m_header.messageId = DISCONNECT;
	response.Serialize(buffer);
	SendPacket(pClient, (char*)&(buffer.m_data[0]), response.m_header.length);
}

void ServerSendManager::BroadcastChannelUpdateMessage(std::string channelName)
{
	ChannelInfo* pInfo = ChannelManager::GetInstance()->GetChannelInfo(channelName);
	pResponseChannel response;
	response.m_header.messageId = CHANGE_NAME;
	response.vecJoinedChannels.push_back(*pInfo);

	list<string>::iterator iter;
	for (iter = pInfo->listClient.begin(); iter != pInfo->listClient.end(); ++iter)
	{
		ClientInfo* client = m_pAuthClient->GetClient(*iter);
		if (nullptr != client)
		{
			Buffer buffer(0);
			response.clientName = client->m_name;
			response.Serialize(buffer);
			SendPacket(client, (char*)&(buffer.m_data[0]), response.m_header.length);
		}
	}
}