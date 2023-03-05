/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description:
#   In this file (AnalizingMessage.h) we have declared our
# all help functions of analizing the message we got and seperate it to type and parameters.
*/

#pragma once
#ifndef ANALIZINGMESSAGE_H
#define ANALIZINGMESSAGE_H


/************************FUNCTIONS**************************/
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we get a pointer to message string, we analyze this message, 
* adding the type we get in the message type feild, and the parameters in the feild of parameters array
*
* Parameters: pointer of message string
*
* Returns: message struct of type and parameters of the message
*
*/
message Get_Message_Details(char* AcceptedStr);
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we get a pointer to string represents the message and 
* a message struct of type and parameters, in this function we count the number of paramerters in the message
* and allocate a memory of message type and message parameter according to its size
*
* Parameters: Pointer of a String message, and a message struct
*
* Returns: number of message parameters
*
*/
int Get_Num_of_Message_parameters(char* AcceptedStr, message* message_details);
/////////////////////////////////////////////////////////////
/*
*
* Description: in this function we get a message struct of type and parameters, and get the number of parameters.
* We add a '\0' at the end of each parameter to make it a string usable in stings functions 
* like strcat function, that we will use these parameters in it.
*
* Parameters: NONE
*
* Returns: message struct, number of parameters
*
*/
void Initialize_Strings_parameters(message* message_details, int num_of_Params);
/////////////////////////////////////////////////////////////

#endif // ANALIZINGMESSAGE_H