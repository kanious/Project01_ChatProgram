#include "LoginDB.h"
#include "AuthServer.h"
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(159);

	AuthServer server;
	printf("Server Opening . . . \n");
	int result = server.AuthServerOpen();
	if (result != 0)
		return result;
	printf("Server Opened.\n\n");

	LoginDB* pDB = new LoginDB();
	string hostName("127.0.0.1:3306");
	string userName("root");
	string password("1qw23er4!Q");
	if (!pDB->Connect(hostName, userName, password))
		return 1;
	pDB->Test();
	//pDB->AddUser();
	//pDB->GetAllUserInfo();
	//pDB->GetUserIdFromName();

	server.Select();

	server.Close();
	pDB->Disconnect();
	delete pDB;

	return 0;
}