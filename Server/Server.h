/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (Server.h) we have declared struct of client parameters, and declared our
# all help functions of Communicating server with clients, creating threads, thread functions,
# calculating the game results functions, accepting sockets, cleaning functions and so on.
*/

#ifndef __SERVER_H_
#define __SERVER_H_

#include <winsock2.h>
#include <stdbool.h>
#include "SocketSendRecvTools.h"
#pragma comment(lib, "ws2_32.lib")

///////////////////////////////////////////////////////

/****************************STRUCTS******************************************/
typedef struct Parameters_to_serviceThread {
	SOCKET socket;

	/********Player********/
	char UserName[USERNAME_MAX_LENGTH];
	char Move[MOVE_MAX_LENGTH];
	char Guess[MOVE_MAX_LENGTH];

	/*******Opponent********/
	char OpponentName[USERNAME_MAX_LENGTH];
	char OpponentMove[MOVE_MAX_LENGTH];
	char OpponentGuess[MOVE_MAX_LENGTH];

	/*****Flags*****/
	bool Opponent_Ready_To_Play;
	bool Opponent_Quit;
	bool Writed_to_File;

	struct Parameters_to_serviceThread* opponent;
}Client_Parameters;
/**************************************************************************/

/**********************************FUNCTIONS*******************************/
/*
*
* Description: this function is the main server function, it initializes and creates the socket, creates the mutex,
* calls the thread of checking exit of server, connects with clients, checks received messages of client disconnection, 
* Check if there is available thread for client, if its third clinet and there is more two players, the function denies it
* else it initializes the client parameters, and creates the client threads. in this function after ending all or after an 
* fail, the function exits by cleaning all the server, it terminates thread when needed, closes socket and closing thread 
* handles, and applying WSACleanup. if the function succeed all of this it reruns STATUS_CODE_SUCCESS and ends the program.
* else returns STATUS_CODE_FAILURE.
*
* Parameters: Port
*
* Returns: STATUS_CODE_SUCCESS or STATUS_CODE_FAILURE
*/
int MainServer(char* Port);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function checks if theres a thread finished running and returns its number
*
* Parameters: NONE
*
* Returns: thread number
*/
static int FindFirstUnusedThreadSlot();
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function closes the sockets for each thread and closes the thread
*
* Parameters: NONE
*
* Returns: NONE
*/
static void CleanupWorkerThreads();
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function accepts the socket, it uses the select function that determines the status of sockets
* if it fails returns STATUS_CODE_FAILURE else STATUS_CODE_SUCCESS
*
* Parameters: Socket
*
* Returns: STATUS_CODE_SUCCESS or STATUS_CODE_FAILURE
*/
int Accept_Socket(SOCKET MainSocket);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function is the thread function of exiting server, it checks all the time if 'exit' string written at 
* server console, and retruns exit variable which is a flag equals to 1 that says the server have to exit
*
* Parameters: thread argument
*
* Returns: exit == 1
*/
static DWORD WINAPI ServerExit(LPVOID thread_arg_p);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: This function is the thread functions that represents the clients, it receives the first message from 
* clients, calls the function which get the message details, and calls the communicate function which communicates 
* with the client from there and on. when it returns from the communicate function the thread keep checking and 
* recieving message from the client until the client disconnect and it is done.
*
* Parameters:  Client parameters struct
*
* Returns: STATUS_CODE_SUCCESS or STATUS_CODE_FAILURE
*/
static DWORD WINAPI ServiceThread(Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: This function identifies the type of the message gottem from client and calls the relevant funcion
* for an example in case the client sent CLIENT_SETUP message, it calls the setup function and works properly
* this function is relevant for CLIENT_VERSUS,CLIENT_SETUP,CLIENT_REQUEST message
* other types of messages considered it in other functions and situations. 
*
* Parameters: 1.message struct of type and parameters 2. Client parameters struct
*
* Returns: NONE
*/
void CommunicateWithClient(message* Message, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: This function takes the request of the client and approves it.
* it gets the name user of the client from the message parameters, send an approve message 
* to client, and also sends the main menu message to display it to the client.
*
* Parameters: 1.message struct of type and parameters 2. Client parameters struct
*
* Returns: NONE
*/
void ClientRequest(message* Message, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: This function is the main function of the Game,the clients play one versus the other,the communication 
* between them is by the file GameSession,first in the function we check if there is a file like this if not the 
* thread creates it, own it by mutex,after that the client waits to opponent to join game with an opponent wait 
* thread, if joined the game and the waiting succeded the game begins and send SERVER_INVITE,SERVER_SETUP messages, 
* waits until the client play and write to file and then he plays and write to file,the accessing to file is arranged 
* by mutex and Writed_to_File flag which points that client or opponent writed and finished using file, then it calls 
* relevant funcions to calculate the game results. in case opponent did'nt join(TIMEOUT) or it quits the game, it 
* sends SERVER_NO_OPPONENTS or SERVER_OPPONENT_QUIT messages, and acts properly (send main menu when no opponents)
* 
* Parameters: 1.message struct of type and parameters 2. Client parameters struct
*
* Returns: NONE
*/
void ClientVersus(message* Message, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: This function setups the client number it will play with that its opponent have to guess
* from the message parameter feild to client move feild, and sends to the client SERVER_PLAYER_MOVE_REQUEST message
* for the first time, that means it starts the first round of guessing 
* 
* Parameters: 1.message struct of type and parameters 2. Client parameters struct
*
* Returns: NONE
*/
void ClientSetup(message* Message, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function calculates the game result, it calculates the bulls and cows of client and bulls of
* opponent and checks who won, if there is no winner, it means the players have to continue guessing (KEEP_GUESSING), 
* the function sends to the client the SERVER_GAME_RESULTS with the bulls, cows and the opponent name and guess as we 
* asked to.
*
* Parameters: 1.message struct of type and parameters 2. Client parameters struct
*
* Returns: NONE
*/
int CalculateGameResult(message* Message, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function sends to the client cases of the results game
* in case of winning client or opponent is sends the SERVER_WIN message and relevant parameters to indacte who won
* also sends SERVER_MAIN_MENU message to play again or quit
* in case of draw is sends the SERVER_DRAW message and SERVER_MAIN_MENU message to play again or quit
*
* Parameters: 1.Result of the game 2. Client parameters struct
*
* Returns: NONE
*/
void SendGameResult(int Result, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function is calculating number of the situations where one of the digits in the guessed number 
* is identical to the digit in the original number, and is placed correctly.
*
* Parameters: 1. original string 2. guess string
*
* Returns: number of bulss
*  ( BULLS = one of the digits in the guessed number is identical to the digit in the original number, and is placed correctly.)
*/
int bull_count(char* original_string, char* guess);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function is calculating number of the situations where one of the digits in the guessed number 
* is the same as the digit in the original number, but is located in a different position.
*
* Parameters: 1. original string 2. guess string
*
* Returns: number of cows
*  ( COWS = one of the digits in the guessed number is the same as the digit in the original number, but is located in a different position.)
*/
int cow_count(char* original_string, char* guess);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function is the thread function of waiting opponent. 
* The function waits until the oppoent of the clients inters the game and is ready to play
* It waits a certain fixed time. 
*
* Parameters: Client parameters struct
*
* Returns: STATUS_CODE_SUCCESS
*/
static DWORD WINAPI WaitingForOpponentToJoin(Client_Parameters* Client); 
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function checks if the transfer value of sending or receiving in server is valid (!=TRNS_FAILED)
* if its not it closes the socket the exits.
*
* Parameters: 1. Transfer Result value 2. Client parameters struct
*
* Returns: NONE
*/
void CheckSendRecvReturnValue(TransferResult_t value, Client_Parameters* Client);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function creates the threads,
* it checks if:
* 1. the recevied pointer of thread function is valid (NOT NULL) 2. if succeded to create the thread
* if not it closes all the thread handles, destroys the queue and the lock, closes the handle of turnstile
* frees all the other allocated memory and exits with error code.
*
* Parameters:
* 1.thread function  2.thread identifier 3. arguments to thread function
*
* Returns:
* Handle of the created thread
*/
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPDWORD p_thread_id, LPVOID thread_arg_p);
///////////////////////////////////////////////////////////////////////////

#endif // ____SERVER_H____