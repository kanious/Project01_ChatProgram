#ifndef _REQUESTCHANGENAME_H_
#define _REQUESTCHANGENAME_H_

#include "Packet.h"
class PACKET_DLL pRequestChangeName : public Packet
{
public:
	unsigned int		clientId;
	std::string			changeName;

public:
	explicit pRequestChangeName();
	virtual ~pRequestChangeName();

public:
	virtual void Serialize(Buffer& buffer);
	virtual void Deserialize(char* data, unsigned int length);
};

#endif //_REQUESTCHANGENAME_H_