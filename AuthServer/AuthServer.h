#ifndef _AUTHSERVER_H_
#define _AUTHSERVER_H_

#include "Define.h"

class AuthClientInfo;
class AuthServer
{
private:
	struct addrinfo*				m_info;
	struct addrinfo					m_hints;
	SOCKET							m_socketListen;
	fd_set							m_fdActive;
	fd_set							m_fdRead;

	unsigned int					UserID;
public:
	std::list<AuthClientInfo*>		m_listAuthClients;

public:
	explicit AuthServer();
	~AuthServer();

public:
	int AuthServerOpen();
	void DisconnectClient(unsigned int id);
private:
	int StartUp();
	int CreateSocket();
	int Bind();
	int Listen();
	void Accept();
	void Recv();
	void RemoveDisconnectedClient();
public:
	void Select();
	void Close();

public:
	AuthClientInfo* GetClient(unsigned int id);
};

#endif //_AUTHSERVER_H_