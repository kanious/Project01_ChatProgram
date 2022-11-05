#include "AuthServerInfo.h"
#include "AuthClient.h"

AuthServerInfo::AuthServerInfo()
{
	m_info = nullptr;
	m_ptr = nullptr;
	ZeroMemory(&m_hints, sizeof(m_hints));
	m_pAuthClient = nullptr;
}

AuthServerInfo::~AuthServerInfo()
{
}

int AuthServerInfo::ConnectToServer(const char* ipAddress)
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

void AuthServerInfo::Close()
{
	freeaddrinfo(m_info);
	WSACleanup();
}

int AuthServerInfo::StartUp(const char* ipAddress)
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

	result = getaddrinfo(ipAddress, AUTH_PORT, &m_hints, &m_info);
	if (result != 0)
	{
		printf("AddrInfo Get FAILED : %d\n", result);
		WSACleanup();
		return result;
	}

	return 0;
}

int AuthServerInfo::CreateSocket()
{
	if (nullptr == m_pAuthClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	m_pAuthClient->m_socket = socket(m_info->ai_family, m_info->ai_socktype, m_info->ai_protocol);
	if (m_pAuthClient->m_socket == INVALID_SOCKET)
	{
		printf("FAILED : %d\n", WSAGetLastError());
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}

int AuthServerInfo::Connect()
{
	if (nullptr == m_pAuthClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	int result = connect(m_pAuthClient->m_socket, m_info->ai_addr, (int)m_info->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		printf("FAILED : %d\n", WSAGetLastError());
		closesocket(m_pAuthClient->m_socket);
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}

int AuthServerInfo::IOCTLsocket()
{
	if (nullptr == m_pAuthClient)
	{
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	DWORD NonBlock = 1;
	int result = ioctlsocket(m_pAuthClient->m_socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		printf("FAILED : %d\n", WSAGetLastError());
		closesocket(m_pAuthClient->m_socket);
		freeaddrinfo(m_info);
		WSACleanup();
		return 1;
	}

	return 0;
}