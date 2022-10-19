#include "Buffer.h"
#include "ServerRecvManager.h"
#include "ChannelManager.h"
#include "Server.h"
#include "DLLPacketIncludes.h"
#include "ServerSendManager.h"

using namespace std;
ServerRecvManager* ServerRecvManager::m_pInstance = nullptr;
ServerRecvManager::ServerRecvManager()
{
	m_pServer = nullptr;
	m_data.clear();
	m_bNewPacket = true;
	m_fullLength = 0;
}

ServerRecvManager::~ServerRecvManager()
{
}

ServerRecvManager* ServerRecvManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ServerRecvManager();
	return m_pInstance;
}

void ServerRecvManager::DestroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
void ServerRecvManager::DeserializePacket(char* rawData, int size, int rawIndex)
{
	if (m_data.size() + size < 4)
	{
		AddData(rawData, size, rawIndex);
	}
	else
	{
		if (m_bNewPacket)
		{
			int headerSize = 4 - m_data.size();
			AddData(rawData, headerSize, rawIndex);
			m_fullLength = ReadLengthInfo();
			int remainedSize = size - headerSize;
			if (m_fullLength <= remainedSize + m_data.size())
			{
				int additionalSize = m_fullLength - m_data.size();
				AddData(rawData, additionalSize, rawIndex + headerSize);
				int nextRawIndex = headerSize + additionalSize;
				ProcessMessage();
				ResetMessageStatus();

				if (size > nextRawIndex)
				{
					DeserializePacket(rawData, size - nextRawIndex, nextRawIndex + rawIndex);
				}
			}
			else
			{
				m_bNewPacket = false;
				AddData(rawData, remainedSize, headerSize + rawIndex);
			}
		}
		else
		{
			if (m_fullLength <= size + m_data.size())
			{
				int needSize = m_fullLength - m_data.size();
				AddData(rawData, needSize, rawIndex);
				int nextRawIndex = needSize;
				ProcessMessage();
				ResetMessageStatus();

				if (size > nextRawIndex)
				{
					DeserializePacket(rawData, size - nextRawIndex, nextRawIndex + rawIndex);
				}
			}
		}
	}
}

void ServerRecvManager::AddData(char* rawData, int size, int rawIndex)
{
	int prevIndex = m_data.size();
	m_data.resize(size + m_data.size());
	memcpy(&(m_data[prevIndex]), &(rawData[rawIndex]), size);
}

void ServerRecvManager::ProcessMessage()
{
	MessageType type = (MessageType)ReadMessageId();

	switch (type)
	{
	case SEND_MSG:			SendMessageRequest();		break;
	case MAKE_CHANNEL:		MakeChannelRequest();		break;
	case DELETE_CHANNEL:	DeleteChannelRequest();		break;
	case JOIN_CHANNEL:		JoinChannelRequest();		break;
	case LEAVE_CHANNEL:		LeaveChannelRequest();		break;
	case DISCONNECT:		DisconnectRequest();		break;
	case CHANGE_NAME:		ChangeNameRequest();		break;
	}
}

void ServerRecvManager::ResetMessageStatus()
{
	m_fullLength = 0;
	m_data.clear();
	m_bNewPacket = true;
}

uint32_t ServerRecvManager::ReadLengthInfo()
{
	uint32_t value = 0;
	value = m_data[3];
	value |= m_data[2] << 8;
	value |= m_data[1] << 16;
	value |= m_data[0] << 24;
	return value;
}

uint32_t ServerRecvManager::ReadMessageId()
{
	uint32_t value = 0;
	value = m_data[7];
	value |= m_data[6] << 8;
	value |= m_data[5] << 16;
	value |= m_data[4] << 24;
	return value;
}

void ServerRecvManager::SendMessageRequest()
{
	pRequestMessage packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);

	ChannelInfo* pChannel = ChannelManager::GetInstance()->GetChannelInfo(packet.channelName);
	if (nullptr == pChannel)
	{
		string err("Cannot find the channel (");
		err.append(packet.channelName);
		err.append(")");
		ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
		ServerSendManager::GetInstance()->SendErrorMessageToClient(pClient, err);
		return;
	}
	else
	{
		if (pChannel->name == SYSTEM_NAME)
		{
			ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
			ServerSendManager::GetInstance()->SendMessageToClient(pClient, (char*)&(m_data[0]), m_fullLength);
			return;
		}
	}
	ServerSendManager::GetInstance()->BroadcastMessageToClients((char*)&(m_data[0]), m_fullLength);
}
void ServerRecvManager::MakeChannelRequest()
{
	pRequestChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);

	int result = ChannelManager::GetInstance()->MakeChannel(packet.channelName, packet.clientId);
	if (result != 0)
	{
		string err("[Error] Same channel name is already exist (");
		err.append(packet.channelName);
		err.append(")");
		ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
		ServerSendManager::GetInstance()->SendErrorMessageToClient(pClient, err);

		return;
	}
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
	ChannelManager::GetInstance()->JoinChannel(packet.channelName, pClient);
	ServerSendManager::GetInstance()->BroadcastChannelMakeMessage(packet.channelName);
}

void ServerRecvManager::DeleteChannelRequest()
{
	pRequestChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);

	int result = ChannelManager::GetInstance()->DeleteChannel(packet.channelName, packet.clientId);
	if (result != 0)
	{
		string err;
		switch (result)
		{
		case 1:	
			err = "[Error] Cannot find the channel (";
			break;
		case 2:		
			err = "[Error] You are not a master of the channel (";
			break;
		case -1:
			err = "[Error] Unknown Error - trying to deleting channel (";
			break;
		}
		err.append(packet.channelName);
		err.append(")");
		ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
		ServerSendManager::GetInstance()->SendErrorMessageToClient(pClient, err);
		return;
	}
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
	ServerSendManager::GetInstance()->BroadcastChannelDeleteMessage(packet.channelName);
}

void ServerRecvManager::JoinChannelRequest()
{
	pRequestChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
	int result = ChannelManager::GetInstance()->JoinChannel(packet.channelName, pClient);
	if (result != 0)
	{
		string err;
		switch (result)
		{
		case 1:
			err = "[Error] Cannot find the channel (";
			break;
		case 2:
			err = "[Error] You are already in the channel (";
			break;
		case -1:
			err = "[Error] Unknown Error - trying to joining channel (";
			break;
		}
		err.append(packet.channelName);
		err.append(")");
		ServerSendManager::GetInstance()->SendErrorMessageToClient(pClient, err);
		return;
	}
	ServerSendManager::GetInstance()->BroadcastChannelJoinMessage(packet.channelName);
}

void ServerRecvManager::LeaveChannelRequest()
{
	pRequestChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
	int result = ChannelManager::GetInstance()->LeaveChannel(packet.channelName, pClient->m_name);
	if (result != 0)
	{
		string err;
		switch (result)
		{
		case 1:
			err = "[Error] Cannot find the channel (";
			break;
		case 2:
			err = "[Error] You are not in the channel (";
			break;
		case -1:
			err = "[Error] Unknown Error - trying to joining channel (";
			break;
		}
		err.append(packet.channelName);
		err.append(")");
		ServerSendManager::GetInstance()->SendErrorMessageToClient(pClient, err);
		return;
	}
	ServerSendManager::GetInstance()->BroadcastChannelLeaveMessage(pClient, packet.channelName);
}

void ServerRecvManager::DisconnectRequest()
{
	pRequestDisconnect packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);

	vector<ChannelInfo> list;
	ChannelManager::GetInstance()->DisconnectUserFromAllChannnel(pClient->m_name, list);

	if (nullptr != m_pServer)
		m_pServer->DisconnectClient(packet.clientId);

	ServerSendManager::GetInstance()->SendDisconnectResponse(pClient);

	for (int i = 0; i < list.size(); ++i)
		ServerSendManager::GetInstance()->BroadcastChannelLeaveMessage(nullptr, list[i].name);
}

void ServerRecvManager::ChangeNameRequest()
{
	pRequestChangeName packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	ClientInfo* pClient = m_pServer->GetClient(packet.clientId);
	vector<ChannelInfo> list;
	ChannelManager::GetInstance()->ChangeNameFromAllChannel(pClient->m_name, packet.changeName, list);
	pClient->m_name = packet.changeName;

	for (int i = 0; i < list.size(); ++i)
		ServerSendManager::GetInstance()->BroadcastChannelUpdateMessage(list[i].name);
}