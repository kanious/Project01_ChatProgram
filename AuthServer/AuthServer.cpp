#include "AuthServer.h"
#include "AuthClientInfo.h"
#include "Buffer.h"
//#include "AuthAuthServerSendManager.h"
//#include "AuthAuthServerRecvManager.h"

using namespace std;

bool DisconnectFinder(AuthClientInfo* client);

const int SHORT_SIZE = 2;
const int INTEGER_SIZE = 4;
AuthServer::AuthServer()
{
	//AuthServerSendManager::GetInstance()->SetAuthServer(this);
	//AuthServerRecvManager::GetInstance()->SetAuthServer(this);
	m_info = nullptr;
	ZeroMemory(&m_hints, sizeof(m_hints));
	m_socketListen = INVALID_SOCKET;
	FD_ZERO(&m_fdActive);
	FD_ZERO(&m_fdRead);
	m_listAuthClients.clear();

	UserID = 0;
}

AuthServer::~AuthServer()
{
}

int AuthServer::AuthServerOpen()
{
	int result = 0;

	printf("  AuthServer Starting . . . ");
	result = StartUp();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Create Socket . . . ");
	result = CreateSocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Bind Socket . . . ");
	result = Bind();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Listen Socket . . . ");
	result = Listen();
	if (result != 0)
		return result;
	printf("OK!\n");

	return result;
}

void AuthServer::DisconnectClient(unsigned int id)
{
	list<AuthClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_id == id)
		{
			(*iter)->m_bConnected = false;
			return;
		}
	}
}

int AuthServer::StartUp()
{
	WSAData wsaData;
	int result = 0;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup FAILED : %d\n", result);
		return result;
	}

	//AuthServer Setting
	m_hints.ai_family = AF_INET;		// IPv4 (for.IPv6-AF_INET6
	m_hints.ai_socktype = SOCK_STREAM;	// Stream (for.UDP-SOCK_DGRAM)
	m_hints.ai_protocol = IPPROTO_TCP;	// TCP
	m_hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, AUTH_PORT, &m_hints, &m_info);
	if (result != 0)
	{
		printf("AddrInfo Get FAILED : %d\n", result);
		WSACleanup();
		return result;
	}

	return 0;
}

int AuthServer::CreateSocket()
{
	m_socketListen = socket(m_info->ai_family, m_info->ai_socktype, m_info->ai_protocol);
	if (m_socketListen == INVALID_SOCKET)
	{
		printf("Socket Create FAILED : %d\n", WSAGetLastError());
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}

int AuthServer::Bind()
{
	int result = bind(m_socketListen, m_info->ai_addr, (int)m_info->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		printf("Socket Bind FAILED : %d\n", WSAGetLastError());
		freeaddrinfo(m_info);
		closesocket(m_socketListen);
		WSACleanup();
		return result;
	}

	return 0;
}

int AuthServer::Listen()
{
	int result = listen(m_socketListen, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		printf("Socket Listen FAILED : %d\n", WSAGetLastError());
		freeaddrinfo(m_info);
		closesocket(m_socketListen);
		WSACleanup();
		return result;
	}

	return 0;
}

void AuthServer::Accept()
{
	if (FD_ISSET(m_socketListen, &m_fdRead))
	{
		printf(" * Accept: ");
		SOCKET clientSocket = accept(m_socketListen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
			printf("FAILED * ");
		else
		{
			printf("Succeed * ");
			AuthClientInfo* client = new AuthClientInfo();
			client->m_socket = clientSocket;
			client->m_bConnected = true;
			client->m_id = UserID++;
			m_listAuthClients.push_back(client);
		}
	}
}

void AuthServer::Recv()
{
	list<AuthClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		AuthClientInfo* client = *iter;
		if (!client->m_bConnected)
			continue;

		if (FD_ISSET(client->m_socket, &m_fdRead))
		{
			char buf[CHUNK_SIZE];
			int result = recv(client->m_socket, buf, CHUNK_SIZE, 0);
			if (result == SOCKET_ERROR)
			{
				client->m_bConnected = false;
				return;
			}
			else
			{
				if (result <= 0)
				{
					client->m_bConnected = false;
					return;
				}

				//AuthServerRecvManager::GetInstance()->DeserializePacket(buf, result, 0);
			}
		}
	}
}

void AuthServer::RemoveDisconnectedClient()
{
	m_listAuthClients.erase(
		remove_if(m_listAuthClients.begin(), m_listAuthClients.end(), DisconnectFinder),
		m_listAuthClients.end());
}

void AuthServer::Select()
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500 * 1000;

	int result = 0;

	while (true)
	{
		FD_ZERO(&m_fdRead);
		FD_SET(m_socketListen, &m_fdRead);

		list<AuthClientInfo*>::iterator iter;
		for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
		{
			if ((*iter)->m_bConnected)
				FD_SET((*iter)->m_socket, &m_fdRead);
		}

		result = select(0, &m_fdRead, NULL, NULL, &tv);
		if (result == SOCKET_ERROR)
		{
			printf("Select FAILED : %d\n", WSAGetLastError());
			return;
		}
		printf("%u", (unsigned int)m_listAuthClients.size());

		Accept();
		Recv();
		RemoveDisconnectedClient();
	}
}

void AuthServer::Close()
{
	freeaddrinfo(m_info);
	closesocket(m_socketListen);
	WSACleanup();
	//AuthServerRecvManager::GetInstance()->DestroyInstance();
	//AuthServerSendManager::GetInstance()->DestroyInstance();
	//ChannelManager::GetInstance()->DestroyInstance();

	list<AuthClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if (nullptr != (*iter))
		{
			delete (*iter);
			(*iter) = nullptr;
		}
	}
	m_listAuthClients.clear();
}

AuthClientInfo* AuthServer::GetClient(unsigned int id)
{
	list<AuthClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_id == id)
			return *iter;
	}
	return nullptr;
}

bool DisconnectFinder(AuthClientInfo* pClient)
{
	return !pClient->m_bConnected;
}