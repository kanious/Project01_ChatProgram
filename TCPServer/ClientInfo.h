#ifndef _CLIENTINFO_H_
#define _CLIENTINFO_H_

#include "Define.h"

class ClientInfo
{
public:
	SOCKET				m_socket;
	bool				m_bConnected;
	unsigned int		m_id;
	std::string			m_name;

public:
	explicit ClientInfo() {}
	~ClientInfo() {}
};

#endif //_CLIENTINFO_H_