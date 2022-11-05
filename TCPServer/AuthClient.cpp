#include <thread>
#include "AuthClient.h"
#include "AuthServerInfo.h"

using namespace std;

bool stopRecv = false;
thread* threadObj;
SOCKET AuthClient::m_socket = INVALID_SOCKET;
AuthClient::AuthClient()
{
	m_pServerInfo = nullptr;
}

AuthClient::~AuthClient()
{
}

void AuthClient::ConnectToServer(const char* ipAddress)
{
	if (nullptr != m_pServerInfo)
		return;

	m_pServerInfo = new AuthServerInfo();
	m_pServerInfo->SetAuthClient(this);
	int result = m_pServerInfo->ConnectToServer(ipAddress);
	if (result == 0)
		StartRecvThread();
	else
	{
		delete m_pServerInfo;
		m_pServerInfo = nullptr;
		closesocket(m_socket);
	}
}

void AuthClient::DisconnectToServer()
{
	StopRecvThread();
	if (nullptr != m_pServerInfo)
	{
		m_pServerInfo->Close();
		delete m_pServerInfo;
		m_pServerInfo = nullptr;
	}
	closesocket(m_socket);
}

void AuthClient::Recv()
{
	char buf[CHUNK_SIZE];
	int result = recv(m_socket, buf, CHUNK_SIZE, 0);
	if (result != SOCKET_ERROR)
	{
		if (result <= 0)
			return;

		//AuthClientRecvManager::GetInstance()->DeserializePacket(buf, result, 0);
	}
}

void AuthClient::Close()
{
	if (nullptr != m_pServerInfo)
	{
		m_pServerInfo->Close();
		delete m_pServerInfo;
		m_pServerInfo = nullptr;
	}
	closesocket(m_socket);
	stopRecv = true;

	//AuthClientRecvManager::GetInstance()->DestroyInstance();
	//AuthClientSendManager::GetInstance()->DestroyInstance();
}

void thread_Recv()
{
	while (!stopRecv)
	{
		AuthClient::Recv();
	}
}

void AuthClient::StartRecvThread()
{
	stopRecv = false;
	threadObj = new thread(thread_Recv);
}

void AuthClient::StopRecvThread()
{
	stopRecv = true;
}
