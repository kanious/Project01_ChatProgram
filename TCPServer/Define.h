#ifndef _DEFINE_H_
#define _DEFINE_H_

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <unordered_map>
#include "DLLdefine.h"

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define SYSTEM_ID 0
#define SYSTEM_CHANNEL "System"
#define SYSTEM_NAME "System"

#endif //_DEFINE_H_