/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (client_function.c) we have declared global variables implemented our
# all help functions of Communicating client with server, creating receiving thread, thread functions,
# functions that controls the TCP masseges from the server to the user .
*/
///////////////////////////////////////////////////////

/****************************Defines******************************************/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

///////////////////////////////////////////////////////

/****************************Laibrary including******************************************/
#include <WinSock2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SocketSendRecvTools.h"
#include "HardCodedData.h"
#include "AnalizingMessage.h"
#include "Client_functions.h"
#include <Windows.h>


//**********************Setting Socket*******************************/
#pragma comment(lib,"ws2_32.lib") //Winsock Library

/*********************************Global Variabels********************/
SOCKET m_socket;
char* User_name;
char* ip_number;
int port_number;

////////////*****************************Functions Of Client System**********************************////////////////////

int Fail_Menu(void)
{
	printf("Choose what to do next:\n");
	printf("1.Try to reconnect\n");
	printf("2.Exit\n");
	char SentStr[2];

	while (1) //ifenite eaiting for the user
	{
		gets_s(SentStr, sizeof(SentStr)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(SentStr, "2"))
			return 2; // User chose 2 option
		else if (STRINGS_ARE_EQUAL(SentStr, "1"))
			return 1; // User chose 1 option
		else
			printf("Please choose a valid option!\n");
	}

}





void Analyze_Server_Message(message* message)// for all kinds of masseges we have different function that handles the cleint response when receiving it
{
	char* main_message = message->Type;
	if (STRINGS_ARE_EQUAL(main_message, SERVER_MAIN_MENU))
	{
		Main_Menu();
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_APPROVED))
	{
		Approved_from_server(message);
		//wee need to do that if we get approved
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_DENIED))
	{
		Connection_Denied(message);
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_INVITE))
	{
		printf("Game is on!\n");
		// in this case we will not do anthing because it is not neccasery (we got information that we will not use it after)
		//the waiting period in another place  we are waiting 
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_SETUP_REQUEST))
	{
		Server_Setup();
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_PLAYER_MOVE_REQUEST))
	{
		Player_Move_Request();
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_GAME_RESULTS))
	{
		Game_Result(message);
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_WIN))
	{
		Win_Situation(message);
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_DRAW))
	{
		Draw_Situation(message);
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_OPPONENT_QUIT))
	{
		Opponent_Quit();
	}
	else if (STRINGS_ARE_EQUAL(main_message, SERVER_NO_OPPONENTS))
	{
		No_Opponent(message);
	}

}


void ReConnect(message* message)//reconecting if the game was ended 
{
	char SendStr[STR_SIZE];
	SOCKADDR_IN clientService;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip_number); //Setting the IP address to connect to
	clientService.sin_port = htons(port_number); //Setting the port to connect to.

	/******************Trying To Connect With Server***********************/
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		Connection_Denied(message);//////deal with situation that not connected
	}
	else
	{
		strcpy(SendStr, CLIENT_REQUEST);//if it (connection) succeded we update the server
		strcat(SendStr, ":");
		strcat(SendStr, User_name);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, m_socket));
	}
}




static DWORD WINAPI RecvDataThread(void)// thread that handles all the recieved data from the server it calls for the analizing to get the right battern to handle it 
{
	TransferResult_t RecvRes;
	while (1)
	{
		char* AcceptedStr = NULL;
		message msg_struct;

		RecvRes = ReceiveString(&AcceptedStr, m_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			msg_struct = Get_Message_Details(AcceptedStr);
			free(AcceptedStr);
		}
		Analyze_Server_Message(&msg_struct);
	}
	return 0;
}






void Main_Menu()// the interface of the user 
{
	int user_cmd_response;
	char SendStr[STR_SIZE];
	printf("Choose what to do next:\n");
	printf("1. Play against another client\n");
	printf("2. Quit\n");
	if (scanf("%d", &user_cmd_response) == EOF) { printf("Failed to get your choice\n"); };

	switch (user_cmd_response)
	{
	case 1:
		strcpy(SendStr, CLIENT_VERSUS);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, m_socket));
		break;
	case 2:
		strcpy(SendStr, CLIENT_DISCONNECT);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, m_socket));
		//we need to stop the client !!!!!!!!!!!!!!!!!!!!!!!!!!
		break;
	}
}



void Approved_from_server(message* message_struct)
{
	printf("Connected to server on %s: %d\n", ip_number, port_number);
}



void Connection_Denied(message* message)
{
	char SendStr[STR_SIZE];
	char user_response;
	if (STRINGS_ARE_EQUAL(message->Parameters[0], "Server Disconnected"))
	{
		printf("Server Disconnected, Game is OVER!\n");
	}
	else
	{	
		printf("Server on <%s>:<%d> denied the connection request.", ip_number, port_number);
		printf("Choose what to do next:\n");
		printf("1. Try to reconnect\n");
		printf("2. Exit\n");

		do
		{
			if (scanf("%1c", &user_response) == EOF) { printf("failed to get your choice\n"); }//// this the case that the player presses enter befor choosing the option 
		} while (user_response == '\n'); 

		if (user_response == '1')
		{
			ReConnect(message);
		}
		else if (user_response == '2')
		{
			FreeMessageStruct(message);
			strcpy(SendStr, CLIENT_DISCONNECT); 
			strcat(SendStr, "\n");
			CheckSendRecvReturnValue(SendString(SendStr, m_socket));

		}
	}
}


void Player_Move_Request()
{
	char SendStr[STR_SIZE];
	char user_gussing[MOVE_MAX_LENGTH];

	printf("Choose your guess:\n");
	if (scanf("%s", user_gussing) == EOF) { printf("FAILED to get your guess\n"); }//////here we get what the player gussing it might be with \n by the player!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	user_gussing[4] = '\0';

	/**************** Setting the messege of the guising number and sending it to Server **********************/
	strcpy(SendStr, CLIENT_PLAYER_MOVE);
	strcat(SendStr, ":");
	strcat(SendStr, user_gussing);
	strcat(SendStr, "\n");
	CheckSendRecvReturnValue(SendString(SendStr, m_socket));
}



void Game_Result(message* message)// the parameters that we get are the results and the player nam and what guessed
{
	printf("Bulls: %s\n", message->Parameters[0]);
	printf("Cows: %s\n", message->Parameters[1]);
	printf("%s played: %s \n", message->Parameters[2], message->Parameters[3]);
}


void Server_Setup()// we setup the connection
{
	char SendStr[STR_SIZE];
	char move[5];
	move[4] = '\0';
	printf("Choose your 4 Digits:\n");
	while (1)
	{
		if (scanf("%s", move) == EOF ) {printf("faild to get the digits\n"); } //////here we get what the player gussing it might be with \n by the player!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (strlen(move) != 4)
		{
			printf("Choose right, choose 4 Digits:\n");
			continue;
		}
		else
		{
			break;
		}
	}
	/**************** Setting the messege of the guising number and sending it to Server **********************/
	strcpy(SendStr, CLIENT_SETUP);
	strcat(SendStr, ":");
	strcat(SendStr, move);
	strcat(SendStr, "\n");
	CheckSendRecvReturnValue(SendString(SendStr, m_socket));
}




void Win_Situation(message* message)//handling win situation 
{
	printf("%s Won!\n", message->Parameters[0]);
	printf("opponents number was: %s\n\n", message->Parameters[1]);
}


void Draw_Situation(message* message)//handling draw situation
{
	printf("It's a tie\n");
}


void Opponent_Quit(void)
{
	printf("Opponent quit!\n");
}

void No_Opponent(message* message)
{
	printf("NO opponent available!\n");
}




int manage_Clientsystem(char* ip, char* port, char* username)// this is the main function that handles the cleint behavior
{
	char SendStrin[STR_SIZE];
	DWORD wait_code;
	SOCKADDR_IN clientService;
	HANDLE recieveHandle; ///Thread for receiving data 

	/////////************************ Initialize Winsock*******************////////////
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");
	ip_number = ip;
	port_number = atoi(port);
	User_name = username;

	//////////***********Creating  Socket************///////////////
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	//******************* Ensure Socket Validation**************////////////
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}

	////************Creation a sockaddr_in object clientService and set  values********************/////
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip_number); //Setting the IP address to connect to
	clientService.sin_port = htons(port_number); //Setting the port to connect to.


	//*******Connection function, passing the created socket and the sockaddr_in structure as parameters*************//// 
retry_connection:
	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		if (Fail_Menu() == 1)
			goto retry_connection;
		WSACleanup();
		return ERROR_CODE;
	}
	//**************connection start between the server and cleint************************/////
	strcpy(SendStrin, CLIENT_REQUEST);
	strcat(SendStrin, ":");
	strcat(SendStrin, User_name);
	strcat(SendStrin, "\n");
	CheckSendRecvReturnValue(SendString(SendStrin, m_socket));

	//***********Activating receiving thread for this cleint***************//
	recieveHandle = CreateThreadSimple((LPTHREAD_START_ROUTINE)RecvDataThread,NULL, NULL);

	wait_code = WaitForSingleObject(recieveHandle, INFINITE);
	if (wait_code != WAIT_OBJECT_0)
	{
		printf("Error while waiting thread\n");
		return ERROR_CODE;
	}
	//************************************Handeling Errors and forcing terminating******************//
	TerminateThread(recieveHandle, 0x555);
	CloseHandle(recieveHandle);
	if (closesocket(m_socket) == SOCKET_ERROR)
	{
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());
		return STATUS_CODE_FAILURE;
	}
	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
		return STATUS_CODE_FAILURE;
	}
	return STATUS_CODE_SUCCESS;
}

static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPDWORD p_thread_id, LPVOID thread_arg_p)
{
	HANDLE thread_handle = NULL;
	if (NULL == p_start_routine)
	{
		// Should not get here
		printf("Error when creating a Thread\n");
		printf("Received null routine pointer\n");
		exit(ERROR_CODE);
	}
	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		thread_arg_p,    /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create Thread\n");
		exit(ERROR_CODE);
	}
	return thread_handle;
}

void CheckSendRecvReturnValue(TransferResult_t value)// function that chicks the status of receiving string 
{
	if (value == TRNS_FAILED)
	{
		printf("Service socket error while sending or receiving in client, closing thread.\n");
		closesocket(m_socket);
		exit(STATUS_CODE_FAILURE);
	}
}



 void FreeMessageStruct(message* message)// destroy all the allocation of memory that have bee created to deal with the message from server
 {
	int parameter_index = 0;
	if (message == NULL)
	{
		return;
	}

	if (message->Type == NULL)
	{
		return;
	}
	else
	{
		free(message->Type);
	}

	for (; parameter_index < MAX_NUM_OF_PARAMETERS; parameter_index++)
	{
		if (message->Parameters[parameter_index] == NULL)
		{
			return;
		}
		else
		{
			free(message->Parameters[parameter_index]);
		}
	}
	return;
}
