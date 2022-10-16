#ifndef _CLIENTSENDMANAGER_H_
#define _CLIENTSENDMANAGER_H_

#include "Define.h"

class Client;
class ClientSendManager
{
private:
	static ClientSendManager*	m_pInstance;

private:
	Client*						m_pClient;

public:
	explicit ClientSendManager();
	~ClientSendManager();
	static ClientSendManager* GetInstance();
	void DestroyInstance();

public:
	void SetClient(Client* pClient) { m_pClient = pClient; }

public:
	void MessageParcing(std::string channelName, std::string message);
	bool CheckServerStatus();
	void SendPacket(char* data, int length);
private:
	void RequestConnectToServer(const char* ipAddress);
	void RequestDisconnectToServer();
	void RequestSendMessageToServer(std::string channelName, std::string message);
	void RequestMakeChannel(std::string channelName);
	void RequestDeleteChannel(std::string channelName);
	void RequestJoinChannel(std::string channelName);
	void RequestLeaveChannel(std::string channelName);
	void RequestChangeName(std::string name);
public:
	void RequestQuit();
};

#endif //_CLIENTSENDMANAGER_H_