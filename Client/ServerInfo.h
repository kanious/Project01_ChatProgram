#ifndef _SERVERINFO_H_
#define _SERVERINFO_H_

#include "Define.h"

class Client;
class ServerInfo
{
private:
	struct addrinfo*		m_info;
	struct addrinfo*		m_ptr;
	struct addrinfo			m_hints;
	Client*					m_pClient;

public:
	explicit ServerInfo();
	~ServerInfo();

public:
	void SetClient(Client* pClient) { m_pClient = pClient; }
	int ConnectToServer(const char* ipAddress);
	void Close();
private:
	int StartUp(const char* ipAddress);
	int CreateSocket();
	int Connect();
	int IOCTLsocket();
};


#endif //_SERVERINFO_H_