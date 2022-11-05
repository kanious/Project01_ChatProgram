#ifndef _DLLDEFINE_H_
#define _DLLDEFINE_H_
#pragma warning(disable : 4251) // STL DLL Warning
#pragma warning(disable : 4267) // size_t Warning
#pragma warning(disable : 4244) // buffer data Warning

#include <iostream>
#include <string>
#include <list>
#include <vector>

#define DEFAULT_PORT "12345"
#define AUTH_PORT "54321"
#define CHUNK_SIZE 128


#ifdef PACKETDLL_EXPORTS
#define PACKET_DLL __declspec(dllexport)
#else
#define PACKET_DLL __declspec(dllimport)
#endif

enum MessageType
{
	CONNECT = 0,
	DISCONNECT,
	ERROR_MSG,
	SEND_MSG,
	MAKE_CHANNEL,
	DELETE_CHANNEL,
	JOIN_CHANNEL,
	LEAVE_CHANNEL,
	CHANGE_NAME
};

#endif //_DLLDEFINE_H_