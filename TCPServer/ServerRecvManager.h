#ifndef _SERVERRECVMANAGER_H_
#define _SERVERRECVMANAGER_H_

#include "Define.h"
#include "ClientInfo.h"

class Server;
class ServerRecvManager
{
private:
	static ServerRecvManager*		m_pInstance;

private:
	Server*							m_pAuthClient;
	std::vector<uint8_t>			m_data;
	bool							m_bNewPacket;
	uint32_t						m_fullLength;

public:
	explicit ServerRecvManager();
	~ServerRecvManager();
	static ServerRecvManager* GetInstance();
	void DestroyInstance();

public:
	void SetServer(Server* pServer) { m_pAuthClient = pServer; }

public:
	void DeserializePacket(char* rawData, int size, int rawIndex);
private:
	void AddData(char* rawData, int size, int rawIndex);
	void ProcessMessage();
	void ResetMessageStatus();
	uint32_t ReadLengthInfo();
	uint32_t ReadMessageId();
private:
	void SendMessageRequest();
	void MakeChannelRequest();
	void DeleteChannelRequest();
	void JoinChannelRequest();
	void LeaveChannelRequest();
	void DisconnectRequest();
	void ChangeNameRequest();
};

#endif //_SERVERRECVMANAGER_H_