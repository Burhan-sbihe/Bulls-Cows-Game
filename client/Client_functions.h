/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description: In this file (client_function.c) we have declared global variables implemented our
# all help functions of Communicating client with server, creating receiving thread, thread functions,
# functions that controls the TCP masseges from the server to the user so on.
*/

#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H
///////////////////////////////////////////////////////

/****************************Laibrary including******************************************/
#include"HardCodedData.h"
#include <stdbool.h>
#include "SocketSendRecvTools.h"

#pragma comment(lib, "ws2_32.lib")

/**********************************FUNCTIONS*******************************/
/*
*
* Description: in this function we are showing the user the main menu and offering to him if to establish the connecting another time 
* in case of failure (No opponent ,Connection FAILED..)
*
* Parameters: NONE
*
* Returns: The choice of the user if to reconnect or to exit from the game 
*/
int Fail_Menu(void);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function deviding the behavior of the cleint system by the message that recieved from the server. 
* it has some batterns to respond for the message from the server , each batterns interactes with the user if needed 
*
* Parameters: struct of message that we get from the server 
*
* Returns: none
*/
void Analyze_Server_Message(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function try to reconnect at the socket with the server if the connection did not established
*
* Parameters: struct of message that we get from the server
*
* Returns: none
*/
void ReConnect(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description: this function we got approvemrnt from the server and we updating the user that is connected to the server succesfuly 
*
* Parameters: struct of message that we get from the server
*
* Returns: none
*/
void Approved_from_server(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:we are showing the user the main menu and offering  him if to play against another player or to exit
*
* Parameters: none
*
* Returns: none
*/
void Main_Menu(void);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:we are showing the user the failing menu and offering  him if to reconnect or to quit we deal with the situation that the server denied or no opponent
* 
* Parameters: struct of message that we get from the server
*
* Returns: none
*/
void Connection_Denied(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the main function that manages the cleint system we create the socket and connecting and creating the receive thread
* and communicate with the server for the first time
*
* Parameters: ip_address,the port , the username of the cleint
*
* Returns: STATUS_CODE_SUCCESS or STATUS_CODE_FAILURE
*/
int manage_Clientsystem(char* ip, char* port, char* username);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals with the request to play a move(server request) so we offering to the user what is his guess 
* then we send the response to the server 
*
* Parameters: None
*
* Returns: None
*/
void Player_Move_Request(void);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals with game_results that we get from the server and forming the right message to the user 
*
* Parameters: struct of message that we get from the server
*
* Returns: None
*/
void Game_Result(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals with Win situation that we get from the server and forming the right message to the user
*
* Parameters: struct of message that we get from the server
*
* Returns: None
*/
void Win_Situation(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals with Draw situation that we get from the server and forming the right message to the user
*
* Parameters: struct of message that we get from the server
*
* Returns: None
*/
void Draw_Situation(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals quiting  situation that we get from the server and forming the right message to the user 
* that the opponent is quit
*
* Parameters: None
*
* Returns: None
*/
void Opponent_Quit(void);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals No opponent  situation that we get from the server and forming the right message to the user
* that the opponent is not available
*
* Parameters:struct of message that we get from the server
*
* Returns: None
*/
void No_Opponent(message* message);

///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function that deals with the cleint choose of his digits that we send to the server and forming the right message to the user
* to ask him what is his choice.
*
* Parameters:None
*
* Returns: None
*/
void Server_Setup(void);

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
/*
*
* Description:this is the  function we chick  the receiveString() return value if it is succeded or not.
*
* Parameters:Messgae struct (TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED) each value has its meaning 
*
* Returns: None
*/
void CheckSendRecvReturnValue(TransferResult_t value);
///////////////////////////////////////////////////////////////////////////
/*
*
* Description:this is the  function we activating a thread that al the time gets strings from the server.
*
* Parameters:None
*
* Returns: DWORD if succeded or not  
*/
static DWORD WINAPI RecvDataThread(void); 
///////////////////////////////////////////////////////////////////////////
/*
*
* Description:destroy all the allocation of memory that have bee created to deal with the message from server and putting the 
* value of NULL to the pointers 
*
* Parameters:struct of message that we get from the server
*
* Returns: None
*/
void FreeMessageStruct(message* message);

#endif // CLIENT_FUNCTIONS_H