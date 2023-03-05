/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description:
#   In this file (AnalizingMessage.c) we have implemented our
# all help functions of analizing the message we got and seperate it to type and parameters. 
*/
#pragma warning(disable:6385)// we used this because the compiler dose not recognize that we allocate opostive ammount of memory 
#pragma warning(disable:6386)// we used this because the compiler dose not recognize that we allocate opostive ammount of memory
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HardCodedData.h"   
#include "AnalizingMessage.h"
#include <Windows.h>



void Initialize_Strings_parameters(message* Message, int Number_of_Parameters) {
	int num = Number_of_Parameters, i = 0;
	while (num != 0) {
		Message->Parameters[i][0] = '\0';
		i++;
		num--;
	}
	return;
}

int Get_Num_of_Message_parameters(char* AcceptedStr, message* message)
{
	int i = 0, Params_num = 0, message_size = 1;

	while (AcceptedStr[i] != '\n') // continue until getting '\n' which means the end of message
	{
		message_size++;
		if (AcceptedStr[i] == ':') // search for message type, which is until ':' 
		{
			message->Type = (char*)malloc(sizeof(char) * message_size); // allocate memory of type size
			if (NULL == message->Type)
			{
				printf("ERROR WHILE ALLOCATING MEMORY\n");
				exit(ERROR_CODE);
			}
			message_size = 1;
			Params_num++;
		}
		if (AcceptedStr[i] == ';') // search for message parameters, which each is until ';' 
		{
			message->Parameters[Params_num - 1] = (char*)malloc(sizeof(char) * message_size); // allocate memory of parameters size
			if (NULL == message->Parameters[Params_num - 1])
			{
				printf("ERROR WHILE ALLOCATING MEMORY\n");
				exit(ERROR_CODE);
			}
			message_size = 1;
			Params_num++;
		}
		i++;
	}
	//allocate memory for the last parameter, which has no ';' at the end
	message->Parameters[Params_num - 1] = (char*)malloc(sizeof(char) * message_size);// allocate memory of parameters size
	if (NULL == message->Parameters[Params_num - 1])
	{
		printf("ERROR WHILE ALLOCATING MEMORY\n");
		exit(ERROR_CODE);
	}
	return Params_num;
}


message Get_Message_Details(char* AcceptedStr) 
{
	message message;
	int c = 0, num_of_parameters, i = 0, flag = 1;
	int str_len = strlen(AcceptedStr);
	char Char_To_Str[2];
	Char_To_Str[1] = '\0'; // add '\0' so we can use it in strcat function

	num_of_parameters = Get_Num_of_Message_parameters(AcceptedStr, &message);
	Initialize_Strings_parameters(&message, num_of_parameters);
	
	while (AcceptedStr[c] != '\n') // '\n' means got the end of message
	{
		while (AcceptedStr[c] != ':' && flag != 0)
		{
			message.Type[c] = AcceptedStr[c]; // update each char in the type feild
			if (AcceptedStr[c + 1] == ':' || AcceptedStr[c + 1] == '\n') {
				message.Type[c + 1] = '\0';
				c++;
				flag = 0; // we use this flag in here to not inter ever again in her by the pervious while of '\n'
				break;
			}
			c++;
		}
		if (AcceptedStr[c] == '\n')// '\n' means got the end of message
		{ 
			break;
		}
		c++;
		while (AcceptedStr[c] != ';') // untill getting the end of one parameter
		{
			Char_To_Str[0] = AcceptedStr[c];
			strcat(message.Parameters[i], Char_To_Str);// update each char in the parameter feild
			if (AcceptedStr[c + 1] == ';' || AcceptedStr[c + 1] == '\n') {
				strcat(message.Parameters[i], "");
				c++;
				break;
			}
			c++;
		}
		i++;
	}
	return message;
}










