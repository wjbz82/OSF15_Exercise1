#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	if(new_matrix == NULL){
		printf("Passed in matrix was not initialized correctly.\n");
		return false; 
	}

	if(strlen(name) == 0){
		printf("No name provided for creation.\n");
		return false; 
	}

	if(rows <= 0 || cols <= 0){
		printf("Invalid amount of rows and / or columns.\n");
		return false; 
	}

	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

	/*
		PURPOSE: This function will, given a matrix, free up its memory usage and remove from the runtime of the program.
		INPUTS: This function takes in 'm', and 'm' being a matrix that is to be removed from program. 
		RETURNS: This function is void, meaning that it returns no value to the caller, that it does some work on passed in data. 
	*/

void destroy_matrix (Matrix_t** m) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	
	if(!m)
		return; 

	free((*m)->data);
	free(*m);
	*m = NULL;
}


	
	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function will determine if two matrices are equal or not.  
		INPUTS: The inputs are a -> matrix one and b -> matrix two
		RETURN: This function returns true if the two matrices are equal, false if something is wrong with the input parameters, and false if they are not equal. 
	*/

bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

	/*
		PURPOSE: This function will take a source matrix, and duplicate it into another matrix, or copy as much as it's able to, meaning that there could be issues with size constraints. 
		INPUTS: The input are a source matrix 'src' and a destination matrix 'dest', both of which are pre-allocated, all that happens is the bytes get copied from one to the other
		RETURNS: This function returns true if the matrices equal, and false if something is wrong with the input parameters, or if they are not equal. 
	*/

bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!dest)
		return false; 

	if (!src) {
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function will iterate over a matrixes content and for each index in the matrix, its value is shifted to the left or right by a certain amount, decided by the user.
		INPUTS: The input are: a -> the matrix to be iterated over and have values adjusted
			direction -> which direction the shift needs to occur in, either l or r
			shift -> how much to shift in each direction
		RETURNS: The function returns a bool, true on function success (after every shift occurs), or false if something goes wrong with the shift, or during examining the function parameters. 
	*/

bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		return false;
	}

	if(direction != 'l' || direction != 'r')
		return false; 

	if(shift < 0)
		return false; 

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->rows; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function takes two matrices and adds them together and stores the result in a 3rd pre-allocated matrix. 
		INPUTS: The input are: a -> one of the two matrices to have its content added with the second matrix and stored in the 3rd matrix
			b -> the second part of the addition command, its contents are taken and added with a's contents
			c -> the final part of this function, and it just takes the contents of a and b added together and stores it
		RETURNS: the function returns a bool, true on function success (when they are finished adding), otherwise false when something goes wrong in the parameters
	*/

bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!a || !b || !c)
		return false; 

	if(!a->data || !b->data || !c->data)
		return false; 

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function takes a matrix and outputs it to the screen for the user to see. 
		INPUTS: The inputs are: m -> the matrix to be iterated over and have its contents displayed to the user
		RETURNS: This function is void, therefore it returns nothing to the user. 
	*/

void display_matrix (Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!m)
		return; 

	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function is nearly the opposite of the write_matrix function, it will open up a file and read the matrix from it into a matrix in the program, the matrix read from the file
			is stored in binary format. 
		INPUTS: The input are: matrix_input_filename -> the filename to be reading the binary-written matrix from.
			m -> the matrix that will have its contents updated with the content read from the file. 
		RETURNS: The function returns a bool, true on successful reading of the file; false on invalid input parameters, or if something goes wrong during reading. 
	*/

bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if(strlen(matrix_input_filename) == 0)
		return false; 

	if(!m)
		return false; 

	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);

	if (close(fd)) {
		return false;

	}
	return true;
}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function will open up a file and write out the matrix to it, in binary. 
		INPUTS: The inputs of the function are: matrix_output_filename -> which is the filename of the file that will have the binary-written matrix saved in
			m -> the matrix to have its contents read and written to the file specified
		RETURNS: The function returns a bool, true on successful writing to the file, and false if something is wrong with parameters; or if something happens during the writing process. 
	*/

bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	if(strlen(matrix_output_filename) == 0)
		return false;

	if(!m)
		return false; 

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	//TODO FUNCTION COMMENT -- parameter checking is done

	/*
		PURPOSE: The purpose of this function is to iterate over each of the fields in the matrix, and generate a random value for it. It also checks to make sure that the start and end range make sense
			e.g. start range is less than end range, otherwise, the values are flipped and the function is re-called. 
		INPUTS: The inputs are: m -> the matrix, which will be iterated over and have random values generated for
			start_range -> the starting range for the random value generator
			end_range -> the end range for the random value generator
		RETURNS: This function returns false if anything goes wrong in the input parameters, but otherwise true when the function ends. 
	*/

bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	if(m == NULL){
		printf("Error: passed in matrix is null; returning.\n");
		return false; 
	}

	if(start_range > end_range){
		printf("Error, ranges are out of place, flip-flopping them.\n");
		int temp = start_range; 
		start_range = end_range; 
		end_range = temp; 
		random_matrix(m, start_range, end_range);
	}

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % end_range + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	//TODO FUNCTION COMMENT
	
	/*
		PURPOSE: This function will "load" a matrix into memory, meaning that it will take an initialized matrix, one that is set to all 0's and fill it with data, that is read in or generated at runtime. 
		INPUTS: The input are m -> the matrix to be populated with data. data -> the data that will be used to populate the matrix.
		RETURNS: This function is a void function, meaning that it returns nothing. It manipulates data, but doesn't return anything, the manipulation is done though pointers. 
	*/

void load_matrix (Matrix_t* m, unsigned int* data) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!m)
		return; 

	if(data == NULL)
		return; 

	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

	//TODO FUNCTION COMMENT

	/*
		PURPOSE: This function will take a passed in matrix and append to the end of the array that is holding or keeping track of all the matrices created (but not deleted) during runtime of the program
		INPUTS: The inputs are: mats -> the array container for the matrices created (but not deleted) during runtime of the program. new_matrix -> the matrix to be added to the array. num_mats -> total
			number of allowed matrices to be appended onto the array
		RETURNS: The function returns a -1 if anything goes wrong with the input parameters; otherwise, it returns the position that the matrix was appended to the array. (Overwrites are allowed).
	*/

unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	if(!mats)
		return - 1; 

	if(!new_matrix)
		return - 1; 

	if(num_mats <= 0)
		return -1; 

	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;

	printf("POS is: %ld\n", pos);

	return pos;
}
