#ifndef _DEFINE_H_
#define _DEFINE_H_

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "DLLdefine.h"

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define MSGBox(msg) MessageBox(NULL, TEXT(msg), TEXT("Info"), MB_OK | MB_ICONEXCLAMATION)


#endif //_DEFINE_H_