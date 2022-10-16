#include "ClientSendManager.h"
#include "Client.h"
#include "Buffer.h"
#include "DLLPacketIncludes.h"

using namespace std;
ClientSendManager* ClientSendManager::m_pInstance = nullptr;
ClientSendManager::ClientSendManager()
{
	m_pClient = nullptr;
}

ClientSendManager::~ClientSendManager()
{
}

ClientSendManager* ClientSendManager::GetInstance()
{
	if (nullptr == m_pInstance)
		m_pInstance = new ClientSendManager();
	return m_pInstance;
}

void ClientSendManager::DestroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ClientSendManager::MessageParcing(string channelName, string message)
{
	if (message[0] == '/')
	{
		string token(message);
		token.erase(0, 1);
		size_t pos = token.find(' ');
		string command = token.substr(0, pos);
		token.erase(0, command.size() + 1);
		
		if ("connect" == command)
		{
			if (token.size() == 0)
				MSGBox("IP Address infomation is missing");
			else
				RequestConnectToServer(token.c_str());
		}
		else
		{
			if (!CheckServerStatus())
			{
				MSGBox("Server is not connected"); 
				return;
			}
		}

		token.erase(remove(token.begin(), token.end(), ' '), token.end());

		if ("disconnect" == command)
		{
			RequestDisconnectToServer();
		}
		else if ("make" == command)
		{
			if (token.size() == 0)
				MSGBox("Channel name is missing");

			RequestMakeChannel(token);
		}
		else if ("delete" == command)
		{
			if (token.size() == 0)
				MSGBox("Channel name is missing");

			RequestDeleteChannel(token);
		}
		else if ("join" == command)
		{
			if (token.size() == 0)
				MSGBox("Channel name is missing");

			RequestJoinChannel(token);
		}
		else if ("leave" == command)
		{
			if (token.size() == 0)
				MSGBox("Channel name is missing");

			RequestLeaveChannel(token);
		}
		else if ("quit" == command)
		{
			RequestQuit();
		}
		else if ("name" == command)
		{
			if (token.size() == 0)
				MSGBox("Name is missing");

			RequestChangeName(token);
		}
	}
	else
	{
		if (!CheckServerStatus())
		{
			MSGBox("Server is not connected");
			return;
		}

		RequestSendMessageToServer(channelName, message);
	}
}

bool ClientSendManager::CheckServerStatus()
{
	if (nullptr == m_pClient)
		return false;

	return m_pClient->GetServerStatus();
}

void ClientSendManager::SendPacket(char* data, int length)
{
	int sendResult = send(m_pClient->m_socket, data, length, 0);
	if (sendResult == SOCKET_ERROR)
		m_pClient->DisconnectToServer();
}

void ClientSendManager::RequestConnectToServer(const char* ipAddress)
{
	if (nullptr == m_pClient)
		return;

	m_pClient->ConnectToServer(ipAddress);
}

void ClientSendManager::RequestDisconnectToServer()
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestDisconnect packet;
	packet.m_header.messageId = DISCONNECT;
	packet.clientId = m_pClient->m_id;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestSendMessageToServer(string channelName, string message)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestMessage packet;
	packet.m_header.messageId = SEND_MSG;
	packet.clientId = m_pClient->m_id;
	packet.channelName = channelName;
	packet.message = message;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestMakeChannel(string channelName)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestChannel packet;
	packet.m_header.messageId = MAKE_CHANNEL;
	packet.clientId = m_pClient->m_id;
	packet.channelName = channelName;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestDeleteChannel(string channelName)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestChannel packet;
	packet.m_header.messageId = DELETE_CHANNEL;
	packet.clientId = m_pClient->m_id;
	packet.channelName = channelName;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestJoinChannel(string channelName)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestChannel packet;
	packet.m_header.messageId = JOIN_CHANNEL;
	packet.clientId = m_pClient->m_id;
	packet.channelName = channelName;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestLeaveChannel(string channelName)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestChannel packet;
	packet.m_header.messageId = LEAVE_CHANNEL;
	packet.clientId = m_pClient->m_id;
	packet.channelName = channelName;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}
void ClientSendManager::RequestChangeName(string name)
{
	if (nullptr == m_pClient)
		return;

	Buffer buffer(0);
	pRequestChangeName packet;
	packet.m_header.messageId = CHANGE_NAME;
	packet.clientId = m_pClient->m_id;
	packet.changeName = name;
	packet.Serialize(buffer);

	SendPacket((char*)&(buffer.m_data[0]), packet.m_header.length);
}

void ClientSendManager::RequestQuit()
{
	RequestDisconnectToServer();

	if (nullptr == m_pClient)
		return;
	m_pClient->QuitProgram();
}
