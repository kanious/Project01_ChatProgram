#ifndef _AUTHCLIENTINFO_H_
#define _AUTHCLIENTINFO_H_

#include "Define.h"

class AuthClientInfo
{
public:
	SOCKET				m_socket;
	bool				m_bConnected;
	unsigned int		m_id;

public:
	explicit AuthClientInfo() {}
	~AuthClientInfo() {}
};

#endif //_AUTHCLIENTINFO_H_