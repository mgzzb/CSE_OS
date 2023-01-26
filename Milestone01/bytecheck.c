/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: bytecheck.c
 * Date Created: Jan 22, 2023
 * File Contents:
    Confirm that the file is under a given size 
    Count how often a particular byte value occurs in a file    
 **********************************/

// Include required libraries
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main( const int argc, const char* argv[] ){

	// If argc less than 2, enter for loop
	if (argc < 2) {
		// Print error and exit program
		fprintf(stdout, "Error, you did not enter a file name.\n");
		exit(EXIT_FAILURE);
	}

	// If argc is equal, enter for loop
	if (argc == 2) {
		// Print error and exit program
		fprintf(stdout, "Error, you did not enter a hexadecimal value. Please enter 3 arguments.\n");
		exit(EXIT_FAILURE);
	}

	// If argc is greater than 3, enter for loop
	if (argc > 3) {
		// Print error and exit program
		fprintf(stdout, "Error, you entered the wrong number of inputs (more than 3). Please enter 3.\n");
		exit(EXIT_FAILURE);
	}

	// Check if argv[2] is not a hexadecimal value
	if ( (argv[2][0] != '0') | (argv[2][1] != 'x')) {
		// Print error and exit program
		fprintf(stdout, "Error, you did not enter a hexadecimal value.\n");
		exit(EXIT_FAILURE);	
	}

	// Store argv[1] (file name) as a const char*
	const char* fileName = argv[1];

	// Create a file pointer and store it
	FILE* inputFile = fopen(fileName, "r");

	// Check if file exists
	if (inputFile == NULL) {
		// Print error and exit program
	    fprintf(stdout, "Error: File does not exist\n");
	    exit(EXIT_FAILURE);
	}

	// Check if file is readable
	if (access(fileName, R_OK ) == -1) {
		// Print error and exit program
		fprintf(stdout, "Error: File is not readable\n");
		exit(EXIT_FAILURE);
	}

	// Obtain and store size of file
	fseek(inputFile, 0L, SEEK_END);
	long int fileSize = ftell(inputFile);

	// Check if file size is greater than 25 kB (25,000 bytes)
	if (fileSize > 25000) {
		// Print error and exit program
		fprintf(stdout, "Error: The file is over 25 kB (file size was %ld)", fileSize);
	    exit(EXIT_FAILURE);
	}

	// Rewind the pointer
	rewind(inputFile);

	// Create a buffer using malloc
	char *buffer = malloc(fileSize + 1);

	// Use fread to go through the file and read the contents
	fread(buffer, fileSize, 1, inputFile);

	// Check if fread fails
	if (ferror(inputFile)) {
		// Print error message and exit
    	fprintf(stdout, "Error: Could not read the file\n");
		exit(EXIT_FAILURE);
  	}

	// Initialize byteCount to zero
	int byteCount = 0;

	// Initialize a long int and use strtol to convert the initial part of the hex value string to a long int 0
	long int hexVal = 0;
	hexVal = strtol(argv[2], 0, 0);

	// Loop through the entire file to check the characters
	for (int charCheck = 0; charCheck < fileSize + 1; charCheck++) {
	
		// Check if current character in buffer is equal to hex value
		if (buffer[charCheck] == hexVal) {
			// Increase byte count
			byteCount += 1;
		}
	}
	
	// Close the file
	fclose(inputFile);

	// Print the final byte count
	fprintf(stdout, "%d\n", byteCount);
	
	// Return EXIT_SUCCESS
	return EXIT_SUCCESS;

}
