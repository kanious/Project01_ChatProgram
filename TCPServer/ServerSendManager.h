#ifndef _SERVERSENDMANAGER_H_
#define _SERVERSENDMANAGER_H_

#include "Define.h"
#include "ClientInfo.h"

class Server;
class ServerSendManager
{
private:
	static ServerSendManager*	m_pInstance;

private:
	Server*						m_pAuthClient;

public:
	explicit ServerSendManager();
	~ServerSendManager();
	static ServerSendManager* GetInstance();
	void DestroyInstance();

public:
	void SetServer(Server* pServer) { m_pAuthClient = pServer; }

public:
	void SendPacket(ClientInfo* pClient, char* data, int length);
	void AcceptClient(ClientInfo* pClient);
	void SendErrorMessageToClient(ClientInfo* pClient, std::string errMsg);
	void SendMessageToClient(ClientInfo* pClient, char* data, unsigned int length);
	void BroadcastMessageToClients(char* data, unsigned int length);
	void SendSystemMessageToClient(ClientInfo* pClient, std::string message);
	void BroadcastChannelMakeMessage(std::string channelName);
	void BroadcastChannelDeleteMessage(std::string channelName);
	void BroadcastChannelJoinMessage(std::string channelName);
	void BroadcastChannelLeaveMessage(ClientInfo* pClient, std::string channelName);
	void SendDisconnectResponse(ClientInfo* pClient);
	void BroadcastChannelUpdateMessage(std::string channelName);
};

#endif //_SERVERSENDMANAGER_H_