/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (Server.c) we have declared global variables implemented our
# all help functions of Communicating server with clients, creating threads, thread functions,
# calculating the game results functions, accepting sockets, cleaning functions and so on.
*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
#include "HardCodedData.h"
#include "Server.h"
#include "TCP.h"
#include "SocketSendRecvTools.h"
#include "AnalizingMessage.h"
#include <Windows.h>

/********************Global Variables************************************/
HANDLE Mutex;
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
int FlagForDisconnecting = FALSE;

/***********************************************************************/

int MainServer(char* Port)
{
	/************************ Initialization ***************************/
	int Ind, NumConnectedClients;
	char SendStr[STR_SIZE];
	BOOL ret_val;
	DWORD exit_code, wait_code;
	SOCKET MainSocket = INVALID_SOCKET;
	int bindRes, Accepted;
	int ListenRes;
	HANDLE Exit_ThreadHandle = NULL;
	SOCKADDR_IN service;
	SOCKET AcceptSocket = INVALID_SOCKET;
	Client_Parameters Client[2]; 
	Client_Parameters* Client_ptr = NULL;
	/******************************************************************/
	srand((unsigned int)time(NULL));

	Initialize_WinSock();

	Client[0].opponent = &(Client[1]); // pointer to the opponent struct 
	Client[1].opponent = &(Client[0]); // pointer to the opponent struct

	MainSocket = Create_Socket();
	if (MainSocket == INVALID_SOCKET)
	{ // if failed the relevant message should be printed in the Create_Socket() function
		goto server_cleanup_1;
	}

	service = Get_Service(Port);
	if (service.sin_addr.s_addr == INADDR_NONE)
	{// the relevant message if failed should be printed in the Get_Service() function
		goto server_cleanup_2;
	}
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		ThreadHandles[Ind] = NULL;
	}
	printf("Waiting for a client to connect...\n");

	// Creating a thread that always checks if exit have been written at the console of server 
	Exit_ThreadHandle = CreateThreadSimple(ServerExit, NULL, NULL);

	/*************** crating mutex to arrange accessing the help file *******************/
	Mutex = CreateMutex(NULL, FALSE, NULL);
	if (Mutex == NULL) {
		printf("ERROR while creating Mutex\n : %d", GetLastError());
		goto server_cleanup_2;
	}/**********************************************************************************/
AcceptOrExit:
	while (1) {
		/************************** Check the exit of server *****************************************/
		ret_val = GetExitCodeThread(Exit_ThreadHandle, &exit_code);
		Sleep(10);
		if (exit_code == 1)
		{
			wait_code = WaitForSingleObject(Exit_ThreadHandle, 0);
			if (wait_code == WAIT_OBJECT_0) 
			{
				CloseHandle(Exit_ThreadHandle);
			}
			if (FlagForDisconnecting == FALSE) // this means client did not disconnected but the server exited
			{	strcpy(SendStr, SERVER_DENIED);
				strcat(SendStr, ":Server Disconnected\n");
				if (SendString(SendStr, AcceptSocket) == TRNS_FAILED)
				{
					printf("Service socket error while sending in server, closing thread.\n"); goto server_cleanup_3;
				}
				Sleep(3000); /* Waiting untill the client gots the disconnet message */	}
			goto server_cleanup_3;
		}/******************************************************************************************/
		Accepted = Accept_Socket(MainSocket);
		if (Accepted == STATUS_CODE_SUCCESS) {
			AcceptSocket = accept(MainSocket, NULL, NULL);
			if (AcceptSocket == INVALID_SOCKET) {
				printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
				goto server_cleanup_3;}
		}
		else {
			goto AcceptOrExit;
		}
		/**********Check if there is available thread for client, if its not third cliet deny**********/
		NumConnectedClients = FindFirstUnusedThreadSlot();
		if (NumConnectedClients == NUM_OF_WORKER_THREADS) { //no slot is available
			printf("Client Denied.\n");
			strcpy(SendStr, SERVER_DENIED);
			strcat(SendStr, ":Dropping Connection No Slots Available\n");
			if (SendString(SendStr, AcceptSocket) == TRNS_FAILED)
			{
				printf("Service socket error while sending in server, closing thread.\n"); goto server_cleanup_3;
			}
			closesocket(AcceptSocket); //Closing the socket, dropping the connection.
		}/*****************************************************************************************/
		else { // client connected, initialize client parameters and create thread represents the client
			printf("Client Connected.\n");
			Client[NumConnectedClients].socket = AcceptSocket;
			Client[NumConnectedClients].Opponent_Ready_To_Play = false;
			Client[NumConnectedClients].Writed_to_File = false; 
			Client[NumConnectedClients].Opponent_Quit = false;
			Client_ptr = &(Client[NumConnectedClients]);
			ThreadHandles[NumConnectedClients] = CreateThreadSimple(ServiceThread, NULL, Client_ptr);
		}
	}
/********************************** SERVER Cleanups **********************************************/
server_cleanup_3:
	if (exit_code == 1)
	{
		TerminateThread(ThreadHandles[0], 1);
		TerminateThread(ThreadHandles[1], 1);
	}
	CleanupWorkerThreads();
server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
	{
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());
		return STATUS_CODE_FAILURE;
	}
server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
		return STATUS_CODE_FAILURE;
	}
	return STATUS_CODE_SUCCESS;
/**********************************************************************************************/
}

static int FindFirstUnusedThreadSlot()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}
	return Ind;
}


static void CleanupWorkerThreads()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], INFINITE);

			if (Res == WAIT_OBJECT_0)
			{
				closesocket(ThreadInputs[Ind]);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf("Waiting for thread failed. Ending program\n");
			}
		}
	}
}



static DWORD WINAPI ServerExit(LPVOID thread_arg_p)
{
	DWORD exit = 1;
	char ServerConsole_input[STR_SIZE];

	gets_s(ServerConsole_input, sizeof(ServerConsole_input));
	while (1)
	{
		if (STRINGS_ARE_EQUAL(ServerConsole_input, "exit"))
		{
			break;
		}
	}
	return exit;
}


int Accept_Socket(SOCKET MainSocket) {
	fd_set set;
	struct timeval timeout;
	int nReady;
	FD_ZERO(&set); /* clear the set */
	FD_SET(MainSocket, &set); /* add our file descriptor to the set */

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	nReady = select(MainSocket + 1, &set, NULL, NULL, &timeout);
	if (nReady == -1 || nReady == 0)/* an error accured OR a timeout occured */
	{ 
		return STATUS_CODE_FAILURE;
	}
	else
		return STATUS_CODE_SUCCESS;
}

static DWORD WINAPI ServiceThread(Client_Parameters* Client)
{
	BOOL Done = FALSE;
	TransferResult_t RecvRes;
	message Message;

	while (!Done) // keep receiving yntil its done (client disconnects)
	{
		char* AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, Client->socket); // recieve message from client
		if (RecvRes == TRNS_FAILED)
		{
			printf("Service socket error while reading, closing thread.\n");
			closesocket(Client->socket);
			return STATUS_CODE_FAILURE;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Connection closed while reading, closing thread.\n");
			closesocket(Client->socket);
			return STATUS_CODE_FAILURE;
		}
		else
		{// analyze the message details to the Message struct
			Message = Get_Message_Details(AcceptedStr);
			free(AcceptedStr);
		}
		/******************* Client Disconncetion************************/
		if (STRINGS_ARE_EQUAL(Message.Type, CLIENT_DISCONNECT)) {
			printf("Client Disconnected\n");
			Done = TRUE;
			FlagForDisconnecting = TRUE;
			Client->opponent->Opponent_Quit = true;
			continue;
		}
		CommunicateWithClient(&Message, Client);
	}
	closesocket(Client->socket);
	return STATUS_CODE_SUCCESS;
}

void CommunicateWithClient(message* Message, Client_Parameters* Client) {
	//	int game_result;
	if (STRINGS_ARE_EQUAL(Message->Type, CLIENT_REQUEST))///Here we must check if 2 Clients are already in and Deny The third Cliend 
	{                                                 /// number of clients Connected is Client_parm->number_of_players
		ClientRequest(Message, Client);
	}
	else if (STRINGS_ARE_EQUAL(Message->Type, CLIENT_VERSUS)) 
	{ // This type of message include the CLIENT_SETUP and CLIENT_PLAYER_MOVE types 

		ClientVersus(Message, Client);
	}
	else if (STRINGS_ARE_EQUAL(Message->Type, CLIENT_SETUP))
	{
		ClientSetup(Message, Client);
	}
	else {
		printf("I Don't know this Type Of message\n");
	}
}




int CalculateGameResult(message* Message, Client_Parameters* Client)
{
	char SendStr[STR_SIZE];
	char ClientBulls[2], ClientCows[2], OpponentBulls;

	ClientBulls[0] = bull_count(Client->OpponentMove, Client->Guess) + '0'; // turn int into char
	ClientCows[0] = cow_count(Client->OpponentMove, Client->Guess) + '0'; // turn int into char
	ClientBulls[1] = '\0';
	ClientCows[1] = '\0';

	OpponentBulls = bull_count(Client->Move, Client->OpponentGuess) + '0'; // turn int into char

	if (ClientBulls[0] == OpponentBulls && ClientBulls[0] == '4')
	{
		printf("Game Result: DRAW\n");
		return DRAW;
	}
	else if (ClientBulls[0] == '4')
	{
		// client win
		printf("Game Result: Client WIN\nGAME OVER!\n\n");		
		return CLIENT_WIN;
	}
	else if (OpponentBulls == '4')
	{
		return OPPONENT_WIN;
	}
	else
	{	// Message to be sent =>  "  massgeType: bulls;cows;OpponentUserName;OpponentGuess \n  "
		printf("Game Result: KEEP GUESSING \n");
		strcpy(SendStr, SERVER_GAME_RESULTS);
		strcat(SendStr, ":");
		strcat(SendStr, ClientBulls);
		strcat(SendStr, ";");
		strcat(SendStr, ClientCows);
		strcat(SendStr, ";");
		strcat(SendStr, Client->OpponentName);
		strcat(SendStr, ";");
		strcat(SendStr, Client->OpponentGuess);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket),Client);
		return KEEP_GUESSING;
	}
}

void SendGameResult(int Result, Client_Parameters* Client)
{
	char SendStr[STR_SIZE];

	switch (Result) {
	case DRAW: //Message to be sent => " massgeType(SERVER_DRAW) \n" 
		strcpy(SendStr, SERVER_DRAW);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);

		Sleep(10);
		//Message to be sent => " massgeType(SERVER_MAIN_MENU) \n" 
		strcpy(SendStr, SERVER_MAIN_MENU);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
		break;
	case CLIENT_WIN://Message to be sent => " massgeType(SERVER_WIN):ClientUserName;ClientGuess\n" 
		strcpy(SendStr, SERVER_WIN);
		strcat(SendStr, ":");
		strcat(SendStr, Client->UserName);
		strcat(SendStr, ";");
		strcat(SendStr, Client->Guess);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);

		Sleep(10);
		//Message to be sent => " massgeType(SERVER_MAIN_MENU) \n" 
		strcpy(SendStr, SERVER_MAIN_MENU);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
		break;
	case OPPONENT_WIN: // means that client loses
		//Message to be sent => " massgeType(SERVER_WIN):ClientOpponentName;ClientOpponentMove\n" 
		strcpy(SendStr, SERVER_WIN); // opponent wins
		strcat(SendStr, ":");
		strcat(SendStr, Client->OpponentName);
		strcat(SendStr, ";");
		strcat(SendStr, Client->OpponentMove);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);

		Sleep(10);
		//Message to be sent => " massgeType(SERVER_MAIN_MENU) \n" 
		strcpy(SendStr, SERVER_MAIN_MENU);
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
		break;
	default:
		break;
	}
}



void ClientRequest(message* Message, Client_Parameters* Client) {
	char SendStr[STR_SIZE];

	strcpy(Client->UserName, Message->Parameters[0]); // setup username
	strcpy(SendStr, SERVER_APPROVED); // send approving message
	strcat(SendStr, "\n");
	CheckSendRecvReturnValue( SendString(SendStr, Client->socket), Client);

	strcpy(SendStr, SERVER_MAIN_MENU); // sed main menu message
	strcat(SendStr, "\n");
	CheckSendRecvReturnValue( SendString(SendStr, Client->socket), Client);
}

void ClientSetup(message* Message, Client_Parameters* Client)
{
	char SendStr[STR_SIZE];
	strcpy(Client->Move , Message->Parameters[0]); // setup the move
	strcpy(SendStr, SERVER_PLAYER_MOVE_REQUEST); // send the guess message to make the client guess and play
	strcat(SendStr, "\n");
	CheckSendRecvReturnValue (SendString(SendStr, Client->socket), Client);
}

void ClientVersus(message* Message, Client_Parameters* Client)
{	/********************* Initializing vairables ************************************/
	FILE* GameSessionFile;
	char  SendStr[256];
	char* AcceptedStr = NULL;
	DWORD wait_code;
	int GameResult1, GameResult2;
	HANDLE OpponentThreadHandle;
	/*********************************************************************************/
	wait_code = WaitForSingleObject(Mutex, INFINITE);
	GameSessionFile = fopen("GameSession.txt", "r");
	if (GameSessionFile == NULL) // CHECK IF THE FILE EXISTS
	{//********File doesn't exist -> no other player, this thread will create the file*********************
		GameSessionFile = fopen("GameSession.txt", "w"); // the file is created
		fclose(GameSessionFile);// close the file so other player would be able to check if it exists
		ReleaseMutex(Mutex);
		OpponentThreadHandle = CreateThreadSimple(WaitingForOpponentToJoin, NULL, Client);
		wait_code = WaitForSingleObject(OpponentThreadHandle, 15000); // wait for opponent to join the game
		if (wait_code == WAIT_OBJECT_0 && Client->Opponent_Ready_To_Play) // opponent join
		{
			CloseHandle(OpponentThreadHandle);
			/***********************SERVER INVITE ***************************/
			strcpy(SendStr, SERVER_INVITE); // we can start to play
			strcat(SendStr, "\n");
			CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
			printf("Game is ON!\n");
			/****************** SERVER SETUP AND PLAY MOVE *********************/
			strcpy(SendStr, SERVER_SETUP_REQUEST); // send setup request to client 
			strcat(SendStr, "\n");
			CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
			AcceptedStr = NULL;
			CheckSendRecvReturnValue( ReceiveString(&AcceptedStr, Client->socket), Client); // recieve CLIENT_SETUP message
			*Message = Get_Message_Details(AcceptedStr); // analyze the message which its type is CLIENT_SETUP
			free(AcceptedStr);
			// call the inner client setup function that setups the username and sends to client SERVER_PLAYER_MOVE_REQUEST message for the first time
			CommunicateWithClient(Message, Client); 
		Guess_Again1:
			Client->Writed_to_File = false; 
			AcceptedStr = NULL;
			CheckSendRecvReturnValue(ReceiveString(&AcceptedStr, Client->socket), Client); // recieve CLIENT_PLAYER_MOVE message
			*Message = Get_Message_Details(AcceptedStr); // in Message is CLIENT_PLAYER_MOVE message and the guess parameter
			free(AcceptedStr);
			strcpy(Client->Guess, Message->Parameters[0]);
			/*************** LOCK FILE FOR WRITING  ************/
			if (WaitForSingleObject(Mutex, INFINITE) != WAIT_OBJECT_0) { printf(" ERROR WHILE WAITING MUTEX"); exit(ERROR_CODE); }
			Client->Opponent_Ready_To_Play = false;
			GameSessionFile = fopen("GameSession.txt", "a+");
			fprintf(GameSessionFile, "%s %s %s\n", Client->UserName, Client->Move, Client->Guess);
			fclose(GameSessionFile);
			Client->Writed_to_File = true; // done writing to file
			ReleaseMutex(Mutex);
			/*************************Accessing to file after opponent accessed******************************/
			while (!Client->opponent->Writed_to_File) {	/*wait for the first player to write his guess to file*/ }
			if (WaitForSingleObject(Mutex, INFINITE) != WAIT_OBJECT_0) { printf(" ERROR WHILE WAITING MUTEX"); exit(ERROR_CODE); }
			GameSessionFile = fopen("GameSession.txt", "a+");
			while (fgetc(GameSessionFile) != '\n') 
			{/*advance to second line*/}
			if (fscanf(GameSessionFile, "%s %s %s\n", Client->OpponentName, Client->OpponentMove, Client->OpponentGuess) == 0) { printf("ERROR while reading"); exit(ERROR_CODE); }
			fclose(GameSessionFile);
			if (remove("GameSession.txt") != 0) { printf("Removing GameSession.txt file Failed");} 
			ReleaseMutex(Mutex);
			/****************************Game Results*******************************/
			GameResult1 = CalculateGameResult(Message, Client);
			SendGameResult(GameResult1, Client);
			if (GameResult1 == KEEP_GUESSING)
			{
				strcpy(SendStr, SERVER_PLAYER_MOVE_REQUEST); // send setup request to client
				strcat(SendStr, "\n");
				CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
				goto Guess_Again1;
			}
			Client->Writed_to_File = false;
		} //********** NO opponent joined the game OR the opponent QUIT*****************
		else if (wait_code == WAIT_TIMEOUT || Client->Opponent_Quit)
		{
			Client->Opponent_Ready_To_Play = false;
			CloseHandle(OpponentThreadHandle);
			if (WaitForSingleObject(Mutex, INFINITE) != WAIT_OBJECT_0) { printf(" ERROR WHILE WAITING MUTEX"); exit(ERROR_CODE); } // lock mutex to protect removing the file
			if (remove("GameSession.txt") != 0) { printf("Removing GameSession.txt file Failed"); }
			ReleaseMutex(Mutex);
			if (Client->Opponent_Quit == true) {/****************** OPPONENT QUITTED *******************/
				Client->Opponent_Quit = false; //reset for the next game
				strcpy(SendStr, SERVER_OPPONENT_QUIT);
				strcat(SendStr, ":");
				strcat(SendStr, Client->OpponentName);
			}/****************** NO OPPONENT JOINED *******************/
			else { strcpy(SendStr, SERVER_NO_OPPONENTS); printf("NO OPPONENTS\n");
				strcat(SendStr, ": No Opponents\n"); }
			CheckSendRecvReturnValue(SendString(SendStr, Client->socket),Client);
			strcpy(SendStr, SERVER_MAIN_MENU); // send main menu to decide playing with another player or quit
			strcat(SendStr, "\n");
			CheckSendRecvReturnValue(SendString(SendStr, Client->socket), Client);
		}
	}
	else
	{ // the file exists, other player created it, this thread can play with pther player
		fclose(GameSessionFile);
		ReleaseMutex(Mutex);
		Client->opponent->Opponent_Ready_To_Play= true;
		/**********************SERVER INVITE ***************************/
		strcpy(SendStr, SERVER_INVITE); // we can start to play
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue(SendString(SendStr, Client->socket),Client);
		/**********************SERVER SETUP ****************************/
		strcpy(SendStr, SERVER_SETUP_REQUEST); // send setup request to client 
		strcat(SendStr, "\n");
		CheckSendRecvReturnValue( SendString(SendStr, Client->socket),Client);
		AcceptedStr = NULL;
		CheckSendRecvReturnValue(ReceiveString(&AcceptedStr, Client->socket),Client); // recieve CLIENT_SETUP message
		*Message = Get_Message_Details(AcceptedStr);
		free(AcceptedStr);
		CommunicateWithClient(Message, Client); // call the inner client setup function that sends to client SERVER_PLAYER_MOVE_REQUEST message for the first time
		/****************** PLAYER MOVE KEEP GUESSING *******************/
	Guess_Again2:
		Client->Writed_to_File = false;
		AcceptedStr = NULL;
		CheckSendRecvReturnValue( ReceiveString(&AcceptedStr, Client->socket),Client); // recieve CLIENT_PLAYER_MOVE message
		*Message = Get_Message_Details(AcceptedStr); // analyze the Message which is CLIENT_PLAYER_MOVE message and the guess parameter
		free(AcceptedStr);
		strcpy(Client->Guess, Message->Parameters[0]); // get the guess
		while (!Client->opponent->Writed_to_File) {/*wait for the first player to write his guess to file*/ }
		/*************************Accessing to file******************************/
		if (WaitForSingleObject(Mutex, INFINITE) != WAIT_OBJECT_0) { printf(" ERROR WHILE WAITING MUTEX"); exit(ERROR_CODE); } // lock the file for writing
		GameSessionFile = fopen("GameSession.txt", "a+");
		if (fscanf(GameSessionFile, "%s %s %s\n", Client->OpponentName,Client->OpponentMove, Client->OpponentGuess)==0) { printf("ERROR while reading"); exit(ERROR_CODE); }
		fprintf(GameSessionFile, "%s %s %s\n", Client->UserName, Client->Move, Client->Guess);
		fclose(GameSessionFile);
		ReleaseMutex(Mutex);
		Client->Writed_to_File = true;
		/****************************Game Results*******************************/
		GameResult2 = CalculateGameResult(Message, Client);
		SendGameResult(GameResult2, Client);
		if (GameResult2 == KEEP_GUESSING)
		{
			strcpy(SendStr, SERVER_PLAYER_MOVE_REQUEST); // send setup request to client
			strcat(SendStr, "\n");
			CheckSendRecvReturnValue( SendString(SendStr, Client->socket) ,  Client);
			goto Guess_Again2;
		}
		Client->Writed_to_File = false;
	}
}

static DWORD WINAPI WaitingForOpponentToJoin(Client_Parameters* Client)
{
	while (!(Client->Opponent_Quit) && !(Client->Opponent_Ready_To_Play))
	{
		Sleep(10);
	}
	return STATUS_CODE_SUCCESS;
}


int bull_count(char* original_string, char* guess)
{
	unsigned int i;
	int bull = 0;
	for (i = 0; i < strlen(original_string); i++)
	{
		if (original_string[i] == guess[i])
		{
			bull++;
		}

	}
	return bull;
}

int cow_count(char* original_string, char* guess)
{
	unsigned int i, j;
	char chick;
	int index_in_integer;
	int cow = 0;
	int using_table[10] = { 0,0,0,0,0,0,0,0,0,0 };
	for (i = 0; i < strlen(original_string); i++)
	{
		index_in_integer = original_string[i] - '0';//this index is the number exists on the original string , we used this index to fill in the used_table so if we used one time any number will not be counted another time for the cows 
		chick = original_string[i];
		for (j = 0; j < strlen(original_string); j++)
		{
			if ((chick == guess[j]) && (using_table[index_in_integer] == 0) && (i != j))
			{
				cow++;
				using_table[index_in_integer] = 1;
			}
		}
	}
	return cow;
}



void CheckSendRecvReturnValue(TransferResult_t value, Client_Parameters* Client)
{
	if (value == TRNS_FAILED)
	{
		printf("Service socket error while sending or receiving in server, closing thread.\n");
		closesocket(Client->socket);
		exit(STATUS_CODE_FAILURE);
	}
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
