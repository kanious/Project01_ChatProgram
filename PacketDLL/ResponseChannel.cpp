#include "pch.h"
#include "Headers\ResponseChannel.h"
#include "Headers\Buffer.h"

using namespace std;
pResponseChannel::pResponseChannel()
	: channelName(""), clientName("")
{
	vecAllChannels.clear();
	vecJoinedChannels.clear();
}

pResponseChannel::~pResponseChannel()
{
}

void pResponseChannel::Serialize(Buffer& buffer)
{
	unsigned int stringSize = 0;
	for (int i = 0; i < vecAllChannels.size(); ++i)
		stringSize += (unsigned int)vecAllChannels[i].size();

	m_header.length = sizeof(PacketHeader)
		+ sizeof(unsigned int) + channelName.size()
		+ sizeof(unsigned int) + clientName.size()
		+ sizeof(unsigned int)											// vecAllChannels.size()
		+ sizeof(unsigned int) * vecAllChannels.size() + stringSize;	// vecAllChannels: channel name

	stringSize = 0;
	for (int i = 0; i < vecJoinedChannels.size(); ++i)
		stringSize += (unsigned int)vecJoinedChannels[i].name.size();
	m_header.length += sizeof(unsigned int)								// vecChannels.size()
		+ sizeof(unsigned int) * vecJoinedChannels.size()				// vecChannels: master id
		+ sizeof(unsigned int) * vecJoinedChannels.size() + stringSize;	// vecChannels: channel name

	unsigned int clientCount = 0;
	stringSize = 0;
	for (int i = 0; i < vecJoinedChannels.size(); ++i)
	{
		list<string>::iterator iter;
		for (iter = vecJoinedChannels[i].listClient.begin(); iter != vecJoinedChannels[i].listClient.end(); ++iter)
		{
			clientCount++;
			stringSize += (unsigned int)iter->size();
		}
	}
	m_header.length += sizeof(unsigned int)								// vecChannels: vecClient.size()
		+ sizeof(unsigned int) * clientCount + stringSize;				// vecChannels: vecClient: name

	buffer.Resize(m_header.length);
	buffer.WriteUInt(m_header.length);
	buffer.WriteUInt(m_header.messageId);

	buffer.WriteUInt((unsigned int)channelName.size()); buffer.WriteString(channelName);
	buffer.WriteUInt((unsigned int)clientName.size()); buffer.WriteString(clientName);
	buffer.WriteUInt((unsigned int)vecAllChannels.size());
	for (int i = 0; i < vecAllChannels.size(); ++i)
	{
		buffer.WriteUInt((unsigned int)vecAllChannels[i].size()); buffer.WriteString(vecAllChannels[i]);
	}

	buffer.WriteUInt((unsigned int)vecJoinedChannels.size());
	for (int i = 0; i < vecJoinedChannels.size(); ++i)
	{
		buffer.WriteUInt(vecJoinedChannels[i].masterId);
		buffer.WriteUInt((unsigned int)vecJoinedChannels[i].name.size()); buffer.WriteString(vecJoinedChannels[i].name);

		buffer.WriteUInt((unsigned int)vecJoinedChannels[i].listClient.size());
		list<string>::iterator iter;
		for (iter = vecJoinedChannels[i].listClient.begin(); iter != vecJoinedChannels[i].listClient.end(); ++iter)
		{
			buffer.WriteUInt((unsigned int)iter->size()); buffer.WriteString(*iter);
		}
	}
}

void pResponseChannel::Deserialize(char* data, unsigned int length)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);
	m_header.length = buffer.ReadUInt();
	m_header.messageId = buffer.ReadUInt();
	int stringSize = 0;

	stringSize = buffer.ReadUInt(); channelName = buffer.ReadString(stringSize);
	stringSize = buffer.ReadUInt(); clientName = buffer.ReadString(stringSize);
	int containerSize = buffer.ReadUInt();
	for (int i = 0; i < containerSize; ++i)
	{
		stringSize = buffer.ReadUInt(); vecAllChannels.push_back(buffer.ReadString(stringSize));
	}

	containerSize = buffer.ReadUInt();
	int containerSize2 = 0;
	for (int i = 0; i < containerSize; ++i)
	{
		ChannelInfo newChannelInfo;
		newChannelInfo.masterId = buffer.ReadUInt();
		stringSize = buffer.ReadUInt(); newChannelInfo.name = buffer.ReadString(stringSize);

		containerSize2 = buffer.ReadUInt();
		for (int j = 0; j < containerSize2; ++j)
		{
			stringSize = buffer.ReadUInt(); newChannelInfo.listClient.push_back(buffer.ReadString(stringSize));
		}

		vecJoinedChannels.push_back(newChannelInfo);
	}
}