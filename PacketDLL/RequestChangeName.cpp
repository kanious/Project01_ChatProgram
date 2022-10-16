#include "pch.h"
#include "Headers\RequestChangeName.h"
#include "Headers\Buffer.h"

pRequestChangeName::pRequestChangeName()
	: clientId(0), changeName("")
{
}

pRequestChangeName::~pRequestChangeName()
{
}

void pRequestChangeName::Serialize(Buffer& buffer)
{
	m_header.length = sizeof(PacketHeader)
		+ sizeof(clientId)
		+ sizeof(unsigned int) + changeName.size();

	buffer.Resize(m_header.length);
	buffer.WriteUInt(m_header.length);
	buffer.WriteUInt(m_header.messageId);

	buffer.WriteUInt(clientId);
	buffer.WriteUInt((unsigned int)changeName.size()); buffer.WriteString(changeName);
}

void pRequestChangeName::Deserialize(char* data, unsigned int length)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);
	m_header.length = buffer.ReadUInt();
	m_header.messageId = buffer.ReadUInt();
	int stringSize = 0;

	clientId = buffer.ReadUInt();
	stringSize = buffer.ReadUInt(); changeName = buffer.ReadString(stringSize);
}