/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Factori
# Description:
#	In this header file we have defined some parameters of messegaes size, address and port, 
#   parameters represent the win draw lose etc, Server and Client Message Types, And a Message type struct .
*/

#ifndef HARD_CODED_DATA_H
#define HARD_CODED_DATA_H

/**************************DEFINES*************************************/
#define SERVER_ADDRESS_STR "127.0.0.1"
#define SERVER_PORT 8888
#define USERNAME_MAX_LENGTH 20
#define MOVE_MAX_LENGTH 10
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define MESSAGE_TYPE_MAX_LENGTH 20
#define MAX_NUM_OF_PARAMETERS 10
/*********************************************************************/
/********************************************************************/
#define ERROR_CODE -1
#define STATUS_CODE_FAILURE -1
#define STATUS_CODE_SUCCESS 0
#define NUM_OF_WORKER_THREADS 2
#define MAX_LOOPS 3
#define STR_SIZE 256
/********************************************************************/
#define DRAW 0
#define CLIENT_WIN 1
#define OPPONENT_WIN 2
#define KEEP_GUESSING 3

/************************ SERVER MESSAGE TYPE *************************/
#define SERVER_MAIN_MENU "Print Menu" 
#define SERVER_APPROVED "Connected"
#define SERVER_DENIED "Connection Denied"
#define SERVER_INVITE "Invite"
#define SERVER_SETUP_REQUEST "Set the number"
#define SERVER_PLAYER_MOVE_REQUEST "Pick guess"
#define SERVER_GAME_RESULTS "Game Results"
#define SERVER_WIN "Win Result"
#define SERVER_DRAW "Even results"
#define SERVER_OPPONENT_QUIT "Opponent Quit"
#define SERVER_NO_OPPONENTS "No Opponents"
/**********************************************************************/

/*********************** CLIENT MESSAGE TYPE **************************/
#define CLIENT_REQUEST "Connect Request"
#define CLIENT_VERSUS "Client vs Client"
#define CLIENT_PLAYER_MOVE "Client move"
#define CLIENT_SETUP "Setup"
#define CLIENT_DISCONNECT "Disconnect"
/***********************************************************************/


/*************************** STRUCTS ***********************************/
typedef struct Messages {
	char* Type;
	char* Parameters[MAX_NUM_OF_PARAMETERS];
}message;
/**********************************************************************/

#endif // HARD_CODED_DATA_H