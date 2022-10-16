#include "pch.h"
#include "Headers\PacketHeader.h"


PacketHeader::PacketHeader()
	: length(0), messageId(0)
{
}

PacketHeader::~PacketHeader()
{
}

void PacketHeader::Deserialize(char* buffer)
{
	char headerBuffer[8];
	memcpy(headerBuffer, buffer, 8);

	unsigned int value = 0;
	value = buffer[0];
	value |= buffer[1] << 8;
	value |= buffer[2] << 16;
	value |= buffer[3] << 24;
	length = value;

	value = 0;
	value = buffer[4];
	value |= buffer[5] << 8;
	value |= buffer[6] << 16;
	value |= buffer[7] << 24;
	messageId = value;
}