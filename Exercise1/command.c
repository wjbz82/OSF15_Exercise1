#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25

	/*
		PURPOSE: This function takes in the user input, and parses it out into an array of commands to be executed by the program. 
		INPUTS: It takes in an input string, that is the command, such as 'create test 4 4' and a cmd structure pointer that holds a num_cmds variable
			and a cmds array pointed to by a double pointer. 
		RETURNS: This function returns false at any point if anything fails, such as the input being null, or if the entire sturucture pointer is null
	*/

bool parse_user_input (const char* input, Commands_t** cmd) {

	if(strlen(input) == 0){
		printf("Please try again, command was empty.\n");
		return false;
	}

	if(cmd == NULL){
		printf("Structure pointer is null, returning.\n");
		return false; 
	}

	char *string = strdup(input);
	
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

	/*
		PURPOSE: This function destroys all commands currently present inside of the cmd structure, so it can be resued again for another command
		INPUTS: This function takes in the cmd structure pointed to by a double pointer (an array of commands), and iterates over them, freeing the memory that they had previously occupied.
		RETURNS: This function is void, meaning it returns nothing, and only does its job, which is freeing the memory of the commands. 
	*/

void destroy_commands(Commands_t** cmd) {

	if((*cmd)->num_cmds <= 0){
		printf("No commands to destory, all have been destroyed\n");
	}else{
	
		for (int i = 0; i < (*cmd)->num_cmds; ++i) {
			free((*cmd)->cmds[i]);
		}
		free((*cmd)->cmds);
		free((*cmd));
		*cmd = NULL;
	}
}

