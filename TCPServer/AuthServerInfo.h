#ifndef _AUTHSERVERINFO_H_
#define _AUTHSERVERINFO_H_

#include "Define.h"

class AuthClient;
class AuthServerInfo
{
private:
	struct addrinfo*		m_info;
	struct addrinfo*		m_ptr;
	struct addrinfo			m_hints;
	AuthClient*				m_pAuthClient;

public:
	explicit AuthServerInfo();
	~AuthServerInfo();

public:
	void SetAuthClient(AuthClient* pAuthClient) { m_pAuthClient = pAuthClient; }
	int ConnectToServer(const char* ipAddress);
	void Close();
private:
	int StartUp(const char* ipAddress);
	int CreateSocket();
	int Connect();
	int IOCTLsocket();
};


#endif //_AUTHSERVERINFO_H_