/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (TCP.c) we have implemented our
# all help functions of creating or initializing socket, and getting service. 
*/

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "TCP.h"
#include "HardCodedData.h"

void Initialize_WinSock() 
{
	WSADATA wsaData;
	// Initialize Winsock.
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		exit(ERROR_CODE);
	}
}

SOCKET Create_Socket()
{      
	SOCKET Socket = INVALID_SOCKET;
	// Create a socket. 
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( Socket == INVALID_SOCKET) {
		printf("Error at socket( ): %ld\n", WSAGetLastError());
	}
	return Socket;
}

SOCKADDR_IN Get_Service(char* Port) 
{
	SOCKADDR_IN service;
	unsigned long Address;

	// Create a sockaddr_in object and set its values.
	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n", SERVER_ADDRESS_STR);
	}
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons(atoi(Port));
	return service;
}


