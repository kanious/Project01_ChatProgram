#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Define.h"

class AuthServerInfo;
class AuthClient
{
private:
	AuthServerInfo* m_pServerInfo;

public:
	static SOCKET			m_socket;

public:
	explicit AuthClient();
	~AuthClient();

public:
	void ConnectToServer(const char* ipAddress);
	void DisconnectToServer();

public:
	static void Recv();

public:
	void StartRecvThread();
	void StopRecvThread();
	void Close();
};

#endif //_CLIENT_H_