#include "Server.h"
#include "ClientInfo.h"
#include "AuthClient.h"

using namespace std;
int main(int argc, char** argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(156);

	HWND hConsole = GetConsoleWindow();
	ShowWindow(hConsole, SW_SHOW);

	cout << "Input AuthServer ipAddress> ";
	string ipAddress = "";
	cin >> ipAddress;

	AuthClient client;
	client.ConnectToServer(ipAddress.c_str());

	Server server;
	printf("Server Opening . . . \n");
	int result = server.ServerOpen();
	if (result != 0)
		return result;
	result = server.MakeSystemChannel();
	if (result != 0)
		return result;
	printf("Server Opened.\n\n");

	

	server.Select();
	server.Close();

	return 0;
}