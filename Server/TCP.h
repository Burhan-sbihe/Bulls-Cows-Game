/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (TCP.h) we have declared our
# all help functions of creating or initializing socket, and getting service.
*/

#ifndef TCP_H
#define TCP_H

/************************FUNCTIONS**************************/
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we Initialize Winsock and check if 
* succeded finding a usable WinSock DLL.
*
* Parameters: NONE
*
* Returns: NONE
*
*/
void Initialize_WinSock();
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we are creating a socket and check if
* succeded creating it.
*
* Parameters: NONE
*
* Returns: Socket
*
*/
SOCKET Create_Socket();
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we convert the string "127.0.0.1" into an ip address and check if succeded 
* and did the declaration of sockaddr_in service are used to set up 
* the sockaddr structure: 
* AF_INET is the Internet address family. 
* "127.0.0.1" is the local IP address to which the socket will be bound. 
* 8888 is the port number to which the socket will be bound.
*
* Parameters: Port= "8888"
*
* Returns: Service
*
*/
SOCKADDR_IN Get_Service(char* Port);
/////////////////////////////////////////////////////////////
#endif // TCP_H