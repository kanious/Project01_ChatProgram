#include "pch.h"
#include "Headers\ResponseDisconnect.h"
#include "Headers\Buffer.h"

pResponseDisconnect::pResponseDisconnect()
	: clientId(0)
{
}

pResponseDisconnect::~pResponseDisconnect()
{
}

void pResponseDisconnect::Serialize(Buffer& buffer)
{
	m_header.length = sizeof(PacketHeader)
		+ sizeof(clientId);

	buffer.Resize(m_header.length);
	buffer.WriteUInt(m_header.length);
	buffer.WriteUInt(m_header.messageId);

	buffer.WriteUInt(clientId);
}

void pResponseDisconnect::Deserialize(char* data, unsigned int length)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);
	m_header.length = buffer.ReadUInt();
	m_header.messageId = buffer.ReadUInt();

	clientId = buffer.ReadUInt();
}