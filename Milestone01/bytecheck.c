/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: bytecheck.c
 * Date Created: Jan 22, 2023
 * File Contents:
    Confirm that the file is under a given size 
    Count how often a particular byte value occurs in a file    
 **********************************/
#include <stdlib.h>
#include <stdio.h>

int main( const int argc, const char* argv[] ){

	// If argc less than 2, enter for loop
	if (argc < 2) {
		// Print error and exit program
		fprintf(stdout, "Error, you did not enter a file name.\n");
		exit(EXIT_FAILURE);
	}

	// If argc is greater than 3, enter for loop
	if (argc > 3) {
		// Print error and exit program
		fprintf(stdout, "Error, you entered the wrong number of inputs. Please enter 3.\n");
		exit(EXIT_FAILURE);
	}

	// Check if argv[2] is not a hexadecimal value ***MISSING***

	FILE* inputFile = fopen(argv[1], "r"); // Create FILE* and store file

	// Check if file exists
	if (inputFile == NULL) {
		// Print error and exit program
	    fprintf(stdout, "File does not exist\n");
	    exit(EXIT_FAILURE);
	}

	// Check if file is readable ***MISSING***


	// Obtain and store size of file
	fseek(inputFile, 0L, SEEK_END);
	long int fileSize = ftell(inputFile);

	// Check if file size is greater than 25 kB (25,600 bytes)
	if (fileSize > 25600) {
		// Print error and exit program
		fprintf(stdout, "Error: The file is over 25 kB (file size was %ld)", fileSize);
	    exit(EXIT_FAILURE);
	}

	// Create a buffer using malloc
	char *buffer = malloc(fileSize + 1);

	// Use fread to go through the file and read the contents
	fread(buffer, fileSize, 1, inputFile);

	// Check if fread fails ***MISSING***

	// Initialize byteCount to zero
	int byteCount = 0;

	// Loop through buffer to check how many
	for (int charCheck = 0; charCheck < fileSize + 1; charCheck++) {

		// Check if current character in buffer is equal to hex value
		if (buffer[charCheck] == argv[2]) {
			// Increase byte count
			byteCount += 1;
		}
	}

	// Close the file
	fclose(inputFile);

	// Print the final byte count
	fprintf(stdout, "%d\n", byteCount);
	
	return 0;

}
