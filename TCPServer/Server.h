#ifndef _SERVER_H_
#define _SERVER_H_

#include "Define.h"

class ClientInfo;
class Server
{
private:
	struct addrinfo*			m_info;
	struct addrinfo				m_hints;
	SOCKET						m_socketListen;
	fd_set						m_fdActive;
	fd_set						m_fdRead;

	unsigned int				UserID;
public:
	std::list<ClientInfo*>		m_listClients;

public:
	explicit Server();
	~Server();

public:
	int ServerOpen();
	int MakeSystemChannel();
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
	std::string GetClientName(unsigned int id);
	ClientInfo* GetClient(unsigned int id);
	ClientInfo* GetClient(std::string name);
};

#endif //_SERVER_H_