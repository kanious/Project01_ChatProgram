#include "pch.h"
#include "Headers\RequestChannel.h"
#include "Headers\Buffer.h"

pRequestChannel::pRequestChannel()
	: clientId(0), channelName("")
{
}

pRequestChannel::~pRequestChannel()
{
}

void pRequestChannel::Serialize(Buffer& buffer)
{
	m_header.length = sizeof(PacketHeader)
		+ sizeof(clientId)
		+ sizeof(unsigned int) + channelName.size();

	buffer.Resize(m_header.length);
	buffer.WriteUInt(m_header.length);
	buffer.WriteUInt(m_header.messageId);

	buffer.WriteUInt(clientId);
	buffer.WriteUInt((unsigned int)channelName.size()); buffer.WriteString(channelName);
}

void pRequestChannel::Deserialize(char* data, unsigned int length)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);
	m_header.length = buffer.ReadUInt();
	m_header.messageId = buffer.ReadUInt();
	int stringSize = 0;

	clientId = buffer.ReadUInt();
	stringSize = buffer.ReadUInt(); channelName = buffer.ReadString(stringSize);
}