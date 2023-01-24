/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: bytecheck.c
 * Date Created: Jan 22, 2023
 * File Contents:
    Confirm that the file is under a given size 
    Count how often a particular byte value occurs in a file    
 **********************************/

int main( const int argc, const char* argv[] ){

	// If argc not equal to zero enter for loop
	if (argc != 2) {
		fprintf(stdout, "Error, you did not enter a file name.\n"); // Print error
		exit(EXIT_FAILURE); // exit program
	}

	// Check if file exists
	if (argv[1] == NULL) {
	    fprintf(stdout, "Cannot open file: %s", argv[1]); // Print error
	    exit(EXIT_FAILURE); // exit program
	}

	FILE* input_file = fopen(argv[1], "r"); // Create FILE* and store file

	fclose( input_file );//close file

	return 0;
}
