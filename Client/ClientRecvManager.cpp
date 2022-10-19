#include "ClientRecvManager.h"
#include "Client.h"
#include "GUIManager.h"
#include "DLLPacketIncludes.h"

using namespace std;
ClientRecvManager* ClientRecvManager::m_pInstance = nullptr;
ClientRecvManager::ClientRecvManager()
{
	m_pClient = nullptr;
	m_data.clear();
	m_bNewPacket = true;
	m_fullLength = 0;
}

ClientRecvManager::~ClientRecvManager()
{
}

ClientRecvManager* ClientRecvManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ClientRecvManager();
	return m_pInstance;
}

void ClientRecvManager::DestroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ClientRecvManager::DeserializePacket(char* rawData, int size, int rawIndex)
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

void ClientRecvManager::AddData(char* rawData, int size, int rawIndex)
{
	int prevIndex = m_data.size();
	m_data.resize(size + m_data.size());
	memcpy(&(m_data[prevIndex]), &(rawData[rawIndex]), size);
}

void ClientRecvManager::ProcessMessage()
{
	MessageType type = (MessageType)ReadMessageId();

	switch (type)
	{
	case CONNECT:			ResponseConnect();			break;
	case ERROR_MSG:			ResponseErrorMessage();		break;
	case SEND_MSG:			ResponseRecvMessage();		break;
	case MAKE_CHANNEL:		ResponseMakeChannel();		break;
	case DELETE_CHANNEL:	ResponseDeleteChannel();	break;
	case JOIN_CHANNEL:		ResponseJoinChannel();		break;
	case LEAVE_CHANNEL:		ResponseLeaveChannel();		break;
	case DISCONNECT:		ResponseDisconnect();		break;
	case CHANGE_NAME:		ResponseChangeName();		break;
	}
}

void ClientRecvManager::ResetMessageStatus()
{
	m_fullLength = 0;
	m_data.clear();
	m_bNewPacket = true;
}

uint32_t ClientRecvManager::ReadLengthInfo()
{
	uint32_t value = 0;
	value = m_data[3];
	value |= m_data[2] << 8;
	value |= m_data[1] << 16;
	value |= m_data[0] << 24;
	return value;
}

uint32_t ClientRecvManager::ReadMessageId()
{
	uint32_t value = 0;
	value = m_data[7];
	value |= m_data[6] << 8;
	value |= m_data[5] << 16;
	value |= m_data[4] << 24;
	return value;
}

void ClientRecvManager::ResponseConnect()
{
	pResponseConnect packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	if (nullptr != m_pClient)
	{
		m_pClient->m_id = packet.clientId;
		m_pClient->m_name = packet.clientName;
	}
	for (int i = 0; i < packet.vecAllChannels.size(); ++i)
		GUIManager::GetInstance()->AddAllChannelList(packet.vecAllChannels[i]);
	for (int i = 0; i < packet.vecJoinedChannels.size(); ++i)
		GUIManager::GetInstance()->UpdateJoinedChannelMap(packet.vecJoinedChannels[i]);
}

void ClientRecvManager::ResponseErrorMessage()
{
	pResponseError packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	GUIManager::GetInstance()->AddConvo(packet.clientId, packet.channelName, packet.clientName, packet.errorMsg);
}

void ClientRecvManager::ResponseRecvMessage()
{
	pResponseMessage packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	GUIManager::GetInstance()->AddConvo(packet.clientId, packet.channelName, packet.clientName, packet.message);
}

void ClientRecvManager::ResponseMakeChannel()
{
	pResponseChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	for (int i = 0; i < packet.vecAllChannels.size(); ++i)
		GUIManager::GetInstance()->AddAllChannelList(packet.vecAllChannels[i]);
	for (int i = 0; i < packet.vecJoinedChannels.size(); ++i)
		GUIManager::GetInstance()->UpdateJoinedChannelMap(packet.vecJoinedChannels[i]);
}

void ClientRecvManager::ResponseDeleteChannel()
{
	pResponseChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	GUIManager::GetInstance()->RemoveFromAllChannel(packet.channelName);
}

void ClientRecvManager::ResponseJoinChannel()
{
	pResponseChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	for (int i = 0; i < packet.vecJoinedChannels.size(); ++i)
		GUIManager::GetInstance()->UpdateJoinedChannelMap(packet.vecJoinedChannels[i]);
}

void ClientRecvManager::ResponseLeaveChannel()
{
	pResponseChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	for (int i = 0; i < packet.vecJoinedChannels.size(); ++i)
	{
		ChannelInfo info = packet.vecJoinedChannels[i];
		bool isJoined = false;
		list<string>::iterator iter;
		for (iter = info.listClient.begin(); iter != info.listClient.end(); ++iter)
		{
			if (*iter == m_pClient->m_name)
			{
				isJoined = true;
				break;
			}
		}
		if (isJoined)
			GUIManager::GetInstance()->UpdateJoinedChannelMap(packet.vecJoinedChannels[i]);
		else
			GUIManager::GetInstance()->RemoveJoinedChannel(info.name);
	}
}

void ClientRecvManager::ResponseDisconnect()
{
	if (nullptr != m_pClient)
		m_pClient->DisconnectToServer();

	GUIManager::GetInstance()->InitAllInformation();
}

void ClientRecvManager::ResponseChangeName()
{
	pResponseChannel packet;
	packet.Deserialize((char*)&(m_data[0]), m_fullLength);
	if (nullptr != m_pClient)
		m_pClient->m_name = packet.clientName;
	for (int i = 0; i < packet.vecJoinedChannels.size(); ++i)
		GUIManager::GetInstance()->UpdateJoinedChannelMap(packet.vecJoinedChannels[i]);

	GUIManager::GetInstance()->UpdateSystemChannel(m_pClient->m_name);
}