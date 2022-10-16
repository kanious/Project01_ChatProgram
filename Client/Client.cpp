#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "Client.h"
#include "ClientRecvManager.h"
#include "ClientSendManager.h"
#include "GUIManager.h"
#include "ServerInfo.h"

using namespace std;

void key_callback(GLFWwindow* window, const int key, int scancode, const int action, const int mods);
bool quitProgram = false;
bool stopRecv = false;

thread* threadObj;
SOCKET Client::m_socket = INVALID_SOCKET;
Client::Client()
{
	m_pServerInfo = nullptr;
	m_pWindow = nullptr;
	m_pGUIManager = GUIManager::GetInstance();
	m_id = 0;
	m_name = "Pumpkin Chat";
}

Client::~Client()
{
}

void Client::ConnectToServer(const char* ipAddress)
{
	if (nullptr != m_pServerInfo)
		return;

	m_pServerInfo = new ServerInfo();
	m_pServerInfo->SetClient(this);
	int result = m_pServerInfo->ConnectToServer(ipAddress);
	if (result == 0)
		StartRecvThread();
	else
	{
		MSGBox("Server Connect Failed");
		delete m_pServerInfo;
		m_pServerInfo = nullptr;
		closesocket(m_socket);
	}
}

void Client::DisconnectToServer()
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

void Client::Loop()
{
	while (!glfwWindowShouldClose(m_pWindow) && !quitProgram)
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		if (nullptr != m_pGUIManager)
			m_pGUIManager->RenderUI();

		glfwSetWindowTitle(m_pWindow, m_name.c_str());

		glfwSwapBuffers(m_pWindow);
	}
}

void Client::QuitProgram()
{
	quitProgram = true;
}

void Client::Recv()
{
	char buf[CHUNK_SIZE];
	int result = recv(m_socket, buf, CHUNK_SIZE, 0);
	if (result != SOCKET_ERROR)
	{
		if (result <= 0)
			return;

		ClientRecvManager::GetInstance()->DeserializePacket(buf, result, 0);
	}
}

void Client::Close()
{
	if (nullptr != m_pServerInfo)
	{
		m_pServerInfo->Close();
		delete m_pServerInfo;
		m_pServerInfo = nullptr;
	}
	closesocket(m_socket);

	glfwTerminate();

	stopRecv = true;

	if (nullptr != m_pGUIManager)
	{
		m_pGUIManager->Close();
		m_pGUIManager->DestroyInstance();
		m_pGUIManager = nullptr;
	}

	ClientRecvManager::GetInstance()->DestroyInstance();
	ClientSendManager::GetInstance()->DestroyInstance();
}

int Client::Ready()
{
	glfwInit();
	m_pWindow = glfwCreateWindow(800, 600, "Pumpkin Chat", nullptr, nullptr);
	if (!m_pWindow)
		return 1;
	glfwMakeContextCurrent(m_pWindow);
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		return 2;
	glfwSetKeyCallback(m_pWindow, key_callback);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	int result = m_pGUIManager->Ready(m_pWindow);
	if (result != 0)
		return result;

	return 0;
}

void thread_Recv()
{
	while (!stopRecv)
	{
		Client::Recv();
	}
}

void Client::StartRecvThread()
{
	stopRecv = false;
	threadObj = new thread(thread_Recv);
}

void Client::StopRecvThread()
{
	stopRecv = true;
}

void key_callback(GLFWwindow* window, const int key, int scancode, const int action, const int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		ClientSendManager::GetInstance()->RequestQuit();
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		GUIManager::GetInstance()->PressSendButton();
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		int test[10];
		int err = test[11];
	}
} 