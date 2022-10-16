#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Define.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ServerInfo;
class GUIManager;
class Client
{
private:
	ServerInfo*				m_pServerInfo;
	GLFWwindow*				m_pWindow;
	GUIManager*				m_pGUIManager;
public:
	static SOCKET			m_socket;
	unsigned int			m_id;
	std::string				m_name;

public:
	explicit Client();
	~Client();

public:
	void ConnectToServer(const char* ipAddress);
	void DisconnectToServer();
	void Loop();
	void QuitProgram();

public:
	static void Recv();

public:
	int Ready();
	void StartRecvThread();
	void StopRecvThread();
	void Close();
	bool GetServerStatus() { return m_pServerInfo != nullptr ? true: false; }
};

#endif //_CLIENT_H_