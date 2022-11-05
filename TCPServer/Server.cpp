#include "Server.h"
#include "ClientInfo.h"
#include "Buffer.h"
#include "ServerSendManager.h"
#include "ServerRecvManager.h"
#include "ChannelManager.h"

using namespace std;

bool DisconnectFinder(ClientInfo* client);

const int SHORT_SIZE = 2;
const int INTEGER_SIZE = 4;
Server::Server()
{
	ServerSendManager::GetInstance()->SetServer(this);
	ServerRecvManager::GetInstance()->SetServer(this);
	m_info = nullptr;
	ZeroMemory(&m_hints, sizeof(m_hints));
	m_socketListen = INVALID_SOCKET;
	FD_ZERO(&m_fdActive);
	FD_ZERO(&m_fdRead);
	m_listAuthClients.clear();

	UserID = 1; // 0: system id
}

Server::~Server()
{
}

int Server::ServerOpen()
{
	int result = 0;

	printf("  Server Starting . . . ");
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

int Server::MakeSystemChannel()
{
	ChannelManager::GetInstance()->MakeChannel(SYSTEM_CHANNEL, SYSTEM_ID);

	return 0;
}

void Server::DisconnectClient(unsigned int id)
{
	list<ClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_id == id)
		{
			(*iter)->m_bConnected = false;
			return;
		}
	}
}

int Server::StartUp()
{
	WSAData wsaData;
	int result = 0;
	
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup FAILED : %d\n", result);
		return result;
	}

	//Server Setting
	m_hints.ai_family = AF_INET;		// IPv4 (for.IPv6-AF_INET6
	m_hints.ai_socktype = SOCK_STREAM;	// Stream (for.UDP-SOCK_DGRAM)
	m_hints.ai_protocol = IPPROTO_TCP;	// TCP
	m_hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, DEFAULT_PORT, &m_hints, &m_info);
	if (result != 0)
	{
		printf("AddrInfo Get FAILED : %d\n", result);
		WSACleanup();
		return result;
	}

	return 0;
}

int Server::CreateSocket()
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

int Server::Bind()
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

int Server::Listen()
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

void Server::Accept()
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
			ClientInfo* client = new ClientInfo();
			client->m_socket = clientSocket;
			client->m_bConnected = true;
			client->m_id = UserID++;
			string name("Unknown_");
			name.append(to_string(client->m_id));
			client->m_name = name;
			m_listAuthClients.push_back(client);

			ChannelManager::GetInstance()->JoinChannel(SYSTEM_CHANNEL, client);
			ServerSendManager::GetInstance()->AcceptClient(client);
			ServerSendManager::GetInstance()->SendSystemMessageToClient(client, "Welcome to the Pumpkin Chat!");
		}
	}
}

void Server::Recv()
{
	list<ClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		ClientInfo* client = *iter;
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

				ServerRecvManager::GetInstance()->DeserializePacket(buf, result, 0);
			}
		}
	}
}

void Server::RemoveDisconnectedClient()
{
	m_listAuthClients.erase(
		remove_if(m_listAuthClients.begin(), m_listAuthClients.end(), DisconnectFinder),
		m_listAuthClients.end());
}

void Server::Select()
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500 * 1000;

	int result = 0;

	while (true)
	{
		FD_ZERO(&m_fdRead);
		FD_SET(m_socketListen, &m_fdRead);

		list<ClientInfo*>::iterator iter;
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

void Server::Close()
{
	freeaddrinfo(m_info);
	closesocket(m_socketListen);
	WSACleanup();
	ServerRecvManager::GetInstance()->DestroyInstance();
	ServerSendManager::GetInstance()->DestroyInstance();
	ChannelManager::GetInstance()->DestroyInstance();

	list<ClientInfo*>::iterator iter;
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

string Server::GetClientName(unsigned int id)
{
	string str = "";
	list<ClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_id == id)
			str = (*iter)->m_name;
	}
	return str;
}

ClientInfo* Server::GetClient(unsigned int id)
{
	list<ClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_id == id)
			return *iter;
	}
	return nullptr;
}

ClientInfo* Server::GetClient(std::string name)
{
	list<ClientInfo*>::iterator iter;
	for (iter = m_listAuthClients.begin(); iter != m_listAuthClients.end(); ++iter)
	{
		if ((*iter)->m_name == name)
			return *iter;
	}
	return nullptr;
}

bool DisconnectFinder(ClientInfo* pClient)
{
	return !pClient->m_bConnected;
}