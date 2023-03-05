/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: 
#   In this main file we called the MainServer function, that runs the server, that connect to clients
#	and calls threads that represent the clients and communicate with the server.
*/

#include <stdio.h>
#include "HardCodedData.h"
#include "Server.h"
#include "SocketSendRecvTools.h"

int main(int argc, char* argv[])
{
	if (argc != 2) 
	{
		printf("Please Insert the right arguments!\n");
		return ERROR_CODE;
	}
	if (remove("GameSession.txt") != 0)
	{
		// when the code works properly should not be file so always gettinf in here
		// DO NOTHING
		// THIS IS THE FIRST CHECK of REMOVING file
		//Removing File Failed OR THere's no FILE like This!
	}
	return MainServer(argv[1]);
}