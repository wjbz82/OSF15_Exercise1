#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include<readline/readline.h>

#include "command.h"
#include "matrix.h"

void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, 
			const char* target);

void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats);

	/*
		PURPOSE: This function is the main driver of the entire program, it feeds every other aspect of the program. Meaning it reads in the users' input
			and calls appropriate functions to do certain tasks, and eventually closes out. 
		INPUTS: This program takes the standard argc (argument count, and array of command line arguments argv). In this program in particular, 
			these are not utlized, but rather there for good practice / measure. 
		RETURNS: This function returns what main usually returns, a status code to let the programmer / user know that the program worked as expected. 
			In this case, that value is: 0. 
	*/

int main (int argc, char **argv) {
	srand(time(NULL));		
	char *line = NULL;
	Commands_t* cmd;

	Matrix_t *mats[10];
	memset(&mats,0, sizeof(Matrix_t*) * 10); // IMPORTANT C FUNCTION TO LEARN

	Matrix_t *temp = NULL;
	bool create_result = create_matrix (&temp,"temp_mat", 5, 5);
	if(create_result == false){
		printf("Failed to create the matrix.\n");
		return -1; 
	}

	if(temp == NULL)
		return -1;

	int result = add_matrix_to_array(mats,temp, 10);
	if(result < 0 || result > 9){
		printf("Failed to add matrix to array.\n");
		return -1; 
	}

	//need to make sure we were able to add the matrix to the array of matrices
	//meaning just need to check what the function returns

	int mat_idx = find_matrix_given_name(mats,10,"temp_mat");

	if (mat_idx < 0) {
		perror("PROGRAM FAILED TO INIT\n");
		return -1;
	}
	random_matrix(mats[mat_idx], 1, 10);
	bool write_success = write_matrix("temp_mat", mats[mat_idx]);

	if(write_success == false){
		printf("Failed to write matrix out to file.\n");
		return -1;
	}

	line = readline("> ");
	while (strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("Failed at parsing command\n\n");
		}
		
		if (cmd->num_cmds > 1) {	
			run_commands(cmd,mats,10);
		}
		if (line) {
			free(line);
		}
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	destroy_remaining_heap_allocations(mats,10);
	return 0;	
}

	/*
		PURPOSE: This function compares the first element of the cmd array cmd->cmds to a given set of strings, and if any of the match, 
			a given matrix function / operation is then called.  
		INPUT: This function takes in the cmd structure, which contains a field for the number of cmds currently being executed, and the command array, 
			which holds the commands themselves. It also takes in the array of matrixes, and the total number of matrixes that are present in the array. 
		RETURNS: This function is void, meaning that it doesn't return anything, it just parses out the commands, and evaluates if any of them 
			can be ran, and if so, it runs them and leaves the function, otherwise it does nothing. 
	*/
void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats) {

	if(cmd == NULL){
		printf("Command container was null.\n");
		return; 
	}

	if(mats == NULL){
		printf("No initialized matrixes in the array.\n");
		return; 
	}

	if(num_mats < 0){
		printf("Invalid number of possible matrixes.\n");
		return; 
	}

	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/

			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (mats[idx]);
			}
			else {
				printf("Matrix (%s) doesn't exist\n", cmd->cmds[1]);
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					printf("Failure to create the result Matrix (%s)\n", cmd->cmds[3]);
					return;
				}
			
				int add_result = add_matrix_to_array(mats,c, num_mats);
					if(add_result < 0 || add_result > 9){
						printf("Failed to add matrix to array.\n");
					return; 
					}

				if (! add_matrices(mats[mat1_idx], mats[mat2_idx],c) ) {
					printf("Failure to add %s with %s into %s\n", mats[mat1_idx]->name, mats[mat2_idx]->name,c->name);
					return;	
				}
			}
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 3 && strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if (mat1_idx >= 0 ) {
				Matrix_t* dup_mat = NULL;
				if( !create_matrix (&dup_mat,cmd->cmds[2], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					return;
				}
				bool duplicate_result = duplicate_matrix (mats[mat1_idx], dup_mat);
				
				if(duplicate_result == false){
					printf("Failed to duplicate the matrix.\n");
					return; 
				}

				int duplicate_add_result = add_matrix_to_array(mats,dup_mat,num_mats);
				
				if(duplicate_add_result < 0 || duplicate_add_result > 9){
					printf("Failed to add the matrix to the array.\n");
					return; 
				}else
					printf ("Duplication of %s into %s finished\n", mats[mat1_idx]->name, cmd->cmds[2]);
		}
		else {
			printf("Duplication Failed\n");
			return;
		}
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 2) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				if ( equal_matrices(mats[mat1_idx],mats[mat2_idx]) ) {
					printf("SAME DATA IN BOTH\n");
				}
				else {
					printf("DIFFERENT DATA IN BOTH\n");
				}
			}
			else {
				printf("Equal Failed\n");
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const int shift_value = atoi(cmd->cmds[3]);
		if (mat1_idx >= 0 ) {
			bool shift_result = bitwise_shift_matrix(mats[mat1_idx],cmd->cmds[2][0], shift_value);
			
			if(shift_result == true){
				printf("Failed to shift the matrix.\n");
				return; 
			}else
				printf("Matrix (%s) has been shifted by %d\n", mats[mat1_idx]->name, shift_value);
		}
		else {
			printf("Matrix shift failed\n");
			return;
		}

	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			printf("Read Failed\n");
			return;
		}	
		
		int result_add = add_matrix_to_array(mats,new_matrix, num_mats);
			
		if(result_add < 0 || result_add > 9){
			printf("Failed to add the matrix to the array.\n");
			return; 
		}else{
			printf("Matrix (%s) is read from the filesystem\n", cmd->cmds[1]);	
		}
	}else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx]->name,mats[mat1_idx])) {
			printf("Write Failed\n");
			return;
		}else {
			printf("Matrix (%s) is wrote out to the filesystem\n", mats[mat1_idx]->name);
		}
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN && cmd->num_cmds == 4) {
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);

		bool create_result = create_matrix(&new_mat,cmd->cmds[1],rows, cols);
		if(create_result == false){
			return; 
		}
		int add_result_final = add_matrix_to_array(mats,new_mat,num_mats);
		if(add_result_final > 9 || add_result_final < 0){
			return; 
		}
		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	else if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		bool random_result = random_matrix(mats[mat1_idx],start_range, end_range);
		if(random_result == false)
			return; 
		printf("Matrix (%s) is randomized between %u %u\n", mats[mat1_idx]->name, start_range, end_range);
	}
	else {
		printf("Not a command in this application\n");
	}

}

	/*
		PURPOSE: This function will iterate over all the matrices in the array of matrices, 
			looking for a matrix with a name matchine the name passed into the function. 
		INPUTS: mats -> array of matrices to be examined. num_mats -> total number of matrices that can reside in the array of matrices
			target -> the target name of the matrix we are looking for. 
		RETURNS: This function returns -1 if any of the input parameters are invalid, or if the search is a failure; otherwise, the position in the array that the matchin matrix is found is returned. 
	*/

unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, const char* target) {

	if(mats == NULL){
		printf("Matrix array is null, returning.\n");
		return -1; 
	}

	if(num_mats < 0){
		printf("Invalid number of possible matrixes.\n");
		return -1; 
	}

	if(strlen(target) == 0){
		printf("No target name specified.\n");
		return -1; 
	}

	for (int i = 0; i < num_mats; ++i) {
		if (mats[i] != NULL && strncmp(mats[i]->name,target,strlen(mats[i]->name)) == 0) {
			return i;
		}
	}
	return -1;
}

	/*
		PURPOSE: This function will iterate over the array of matrices and de-allocate the memory set aside for each matrix inside of the array, and then de-allocate the array. 
		INPUTS: The input are: **mats -> double pointer that references an array holding all of the matrices that are generated through runtime of this program, and num_mats is the counter of how many matrices theere are. 
		RETURNS: This function is a void function, meaning that it returns nothing. It does work on some sort of passed in data, and manipulates it through pointers, thus directly modifying the memory. 
	*/

void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats) {

	if(!mats)
		return; 

	if(num_mats <= 0)
		return; 

	for(int i = 0; i < num_mats; i++){
		if(mats[i] != NULL)
			free(mats[i]->data);
			free(mats[i]);
	}
}
