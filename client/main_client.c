/*
# Authors:
#	Burhan sebehye - 318490216
#	Deema Obeid Zoabi - 206372419
# Project:
#	Bulls & Cows Game
# Description:
#   In this file (main_client.c) we are in the main function of client project we are checking if we got right arguments from the user 
#   then we are calling the manage_Clientsystem function that manges the whole systm of the cleint.
*/


#include "Client_functions.h"
#include <stdio.h>
int main(int argc, char* argv[])
{
	if (argc != 4) {
		printf("Please Insert the right arguments!\n");
		return ERROR_CODE;
	}

	return manage_Clientsystem(argv[1], argv[2], argv[3]);
}