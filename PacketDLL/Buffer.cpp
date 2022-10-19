#include "pch.h"
#include "Headers\Buffer.h"

using namespace std;

Buffer::Buffer(size_t size)
	: m_writeBuffer(0), m_readBuffer(0)
{
	m_data.resize(size);
}

Buffer::~Buffer()
{
}

void Buffer::Resize(size_t size)
{
	m_data.resize(size);
}

void Buffer::WriteShort(int16_t value)
{
	if (m_writeBuffer >= m_data.size())
		return;

	m_data[m_writeBuffer + 1] = value;
	m_data[m_writeBuffer] = value >> 8;

	m_writeBuffer += 2;
}

void Buffer::WriteUShort(uint16_t value)
{
	if (m_writeBuffer >= m_data.size())
		return;

	m_data[m_writeBuffer + 1] = value;
	m_data[m_writeBuffer] = value >> 8;

	m_writeBuffer += 2;
}

void Buffer::WriteInt(int32_t value)
{
	if (m_writeBuffer >= m_data.size())
		return;

	m_data[m_writeBuffer + 3] = value;
	m_data[m_writeBuffer + 2] = value >> 8;
	m_data[m_writeBuffer + 1] = value >> 16;
	m_data[m_writeBuffer] = value >> 24;

	m_writeBuffer += 4;
}

void Buffer::WriteUInt(uint32_t value)
{
	if (m_writeBuffer >= m_data.size())
		return;

	m_data[m_writeBuffer + 3] = value;
	m_data[m_writeBuffer + 2] = value >> 8;
	m_data[m_writeBuffer + 1] = value >> 16;
	m_data[m_writeBuffer] = value >> 24;

	m_writeBuffer += 4;
}

void Buffer::WriteString(string value)
{
	if (m_writeBuffer >= m_data.size())
		return;

	string::iterator iter;
	for (iter = value.begin(); iter != value.end(); ++iter)
	{
		m_data[m_writeBuffer] = *iter;
		m_writeBuffer++;
	}
}

int16_t Buffer::ReadShort()
{
	if (m_readBuffer >= m_data.size())
		return 0;

	int16_t value = 0;
	value = m_data[m_readBuffer + 1];
	value |= m_data[m_readBuffer] << 8;

	m_readBuffer += 2;

	return value;
}

uint16_t Buffer::ReadUShort()
{
	if (m_readBuffer >= m_data.size())
		return 0;

	uint16_t value = 0;
	value = m_data[m_readBuffer + 1];
	value |= m_data[m_readBuffer] << 8;

	m_readBuffer += 2;

	return value;
}

int32_t Buffer::ReadInt()
{
	if (m_readBuffer >= m_data.size())
		return 0;

	int32_t value = 0;
	value = m_data[m_readBuffer + 3];
	value |= m_data[m_readBuffer + 2] << 8;
	value |= m_data[m_readBuffer + 1] << 16;
	value |= m_data[m_readBuffer] << 24;

	m_readBuffer += 4;

	return value;
}

uint32_t Buffer::ReadUInt()
{
	if (m_readBuffer >= m_data.size())
		return 0;

	uint32_t value = 0;
	value = m_data[m_readBuffer + 3];
	value |= m_data[m_readBuffer + 2] << 8;
	value |= m_data[m_readBuffer + 1] << 16;
	value |= m_data[m_readBuffer] << 24;

	m_readBuffer += 4;

	return value;
}

string Buffer::ReadString(size_t size)
{
	if (m_readBuffer >= m_data.size())
		return "";

	string str;

	for (size_t i = 0; i < size; ++i)
	{
		str += m_data[m_readBuffer];
		m_readBuffer++;
	}

	return str;
}
