#include "pch.h"
#include "Headers\ResponseError.h"
#include "Headers\Buffer.h"

pResponseError::pResponseError()
	: clientId(0), channelName(""), clientName(""), errorMsg("")
{
}

pResponseError::~pResponseError()
{
}

void pResponseError::Serialize(Buffer& buffer)
{
	m_header.length = sizeof(PacketHeader)
		+ sizeof(unsigned int)
		+ sizeof(unsigned int) + channelName.size()
		+ sizeof(unsigned int) + clientName.size()
		+ sizeof(unsigned int) + errorMsg.size();

	buffer.Resize(m_header.length);
	buffer.WriteUInt(m_header.length);
	buffer.WriteUInt(m_header.messageId);

	buffer.WriteUInt(clientId);
	buffer.WriteUInt((unsigned int)channelName.size()); buffer.WriteString(channelName);
	buffer.WriteUInt((unsigned int)clientName.size()); buffer.WriteString(clientName);
	buffer.WriteUInt((unsigned int)errorMsg.size()); buffer.WriteString(errorMsg);
}

void pResponseError::Deserialize(char* data, unsigned int length)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);
	m_header.length = buffer.ReadUInt();
	m_header.messageId = buffer.ReadUInt();
	int stringSize = 0;

	clientId = buffer.ReadUInt();
	stringSize = buffer.ReadUInt(); channelName = buffer.ReadString(stringSize);
	stringSize = buffer.ReadUInt(); clientName = buffer.ReadString(stringSize);
	stringSize = buffer.ReadUInt(); errorMsg = buffer.ReadString(stringSize);
}