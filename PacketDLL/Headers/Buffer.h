#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "DLLdefine.h"

class PACKET_DLL Buffer
{
public:
	std::vector<uint8_t>			m_data;
	size_t							m_writeBuffer;
	size_t							m_readBuffer;

public:
	explicit Buffer(size_t size);
	virtual ~Buffer();

public:
	void Resize(size_t size);

public:
	void WriteShort(int16_t);
	void WriteUShort(uint16_t);
	void WriteInt(int32_t);
	void WriteUInt(uint32_t);
	void WriteString(std::string);
public:
	int16_t ReadShort();
	uint16_t ReadUShort();
	int32_t ReadInt();
	uint32_t ReadUInt();
	std::string ReadString(size_t);
};

#endif //_BUFFER_H_