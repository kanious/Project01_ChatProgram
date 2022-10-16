#ifndef _CLIENTRECVMANAGER_H_
#define _CLIENTRECVMANAGER_H_

#include "Define.h"

class Client;
class ClientRecvManager
{
private:
	static ClientRecvManager*	m_pInstance;

private:
	Client*						m_pClient;
	std::vector<uint8_t>		m_data;
	bool						m_bNewPacket;
	uint32_t					m_fullLength;

public:
	explicit ClientRecvManager();
	~ClientRecvManager();
	static ClientRecvManager* GetInstance();
	void DestroyInstance();

public:
	void SetClient(Client* pClient) { m_pClient = pClient; }

public:
	void DeserializePacket(char*, int, int);
private:
	void AddData(char*, int, int);
	void ProcessMessage();
	void ResetMessageStatus();
	uint32_t ReadLengthInfo();
	uint32_t ReadMessageId();
private:
	void ResponseConnect();
	void ResponseErrorMessage();
	void ResponseRecvMessage();
	void ResponseMakeChannel();
	void ResponseDeleteChannel();
	void ResponseJoinChannel();
	void ResponseLeaveChannel();
	void ResponseDisconnect();
	void ResponseChangeName();
};

#endif //_CLIENTRECVMANAGER_H_