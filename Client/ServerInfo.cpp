#include "ServerInfo.h"
#include "Client.h"

ServerInfo::ServerInfo()
{
	m_info = nullptr;
	m_ptr = nullptr;
	ZeroMemory(&m_hints, sizeof(m_hints));
	m_pClient = nullptr;
}

ServerInfo::~ServerInfo()
{
}

int ServerInfo::ConnectToServer(const char* ipAddress)
{
	int result = 0;

	printf("  Start Up . . . ");
	result = StartUp(ipAddress);
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Create Socket . . . ");
	result = CreateSocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Connect to server . . . ");
	result = Connect();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  IOCTLsocket . . . ");
	result = IOCTLsocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	return result;
}

void ServerInfo::Close()
{
	freeaddrinfo(m_info);
	WSACleanup();
}

int ServerInfo::StartUp(const char* ipAddress)
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

	result = getaddrinfo(ipAddress/*"127.0.0.1"*/, DEFAULT_PORT, &m_hints, &m_info);
	if (result != 0)
	{
		printf("AddrInfo Get FAILED : %d\n", result);
		WSACleanup();
		return result;
	}

	return 0;
}

int ServerInfo::CreateSocket()
{
	if (nullptr == m_pClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	m_pClient->m_socket = socket(m_info->ai_family, m_info->ai_socktype, m_info->ai_protocol);
	if (m_pClient->m_socket == INVALID_SOCKET)
	{
		printf("Socket Create FAILED : %d\n", WSAGetLastError());
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}

int ServerInfo::Connect()
{
	if (nullptr == m_pClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	int result = connect(m_pClient->m_socket, m_info->ai_addr, (int)m_info->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		printf("Connect to server FAILED : %d\n", WSAGetLastError());
		closesocket(m_pClient->m_socket);
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}

int ServerInfo::IOCTLsocket()
{
	if (nullptr == m_pClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	DWORD NonBlock = 1;
	int result = ioctlsocket(m_pClient->m_socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		printf("IOCTLsocket FAILED : %d\n", WSAGetLastError());
		closesocket(m_pClient->m_socket);
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}