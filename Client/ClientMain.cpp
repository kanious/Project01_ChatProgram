#include "Client.h"
#include "ClientRecvManager.h"
#include "ClientSendManager.h"

int main(int argc, char** argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(509);

	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_HIDE);

	Client client;
	int result = client.Ready();
	if (result != 0)
		return result;
	ClientRecvManager::GetInstance()->SetClient(&client);
	ClientSendManager::GetInstance()->SetClient(&client);

	client.Loop();
	client.Close();

	return 0;
}