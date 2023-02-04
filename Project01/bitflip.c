/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: bitflip.c
 * Date Created: Jan 26, 2023
 * File Contents:
    Level 1: Given a file within a certain size limit (default of 25 kB or 
    specified via argument), output a bit-flipped version of the file with an appended 
    extension (.bf).   
    Level 2: Given a file within a certain size limit (25 kB or specified), 
    output a bit-flipped and reverse byte-ordered file with an appended extension (.rbf 
    or .r).
 **********************************/

// Include required libraries
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

/* Usage function to display executable usage messages and exit */
void usage() {
    fprintf(stderr, "\nBitFlip Executable Usage:\n\n");
    fprintf(stderr, "   -o XXX      Override output file name (XXX is output file name to use), will also override appending of the extension\n");
    fprintf(stderr, "   -maxsize XXX      Override max file size allowed for input (XXX is new max size in bytes)\n");
    fprintf(stderr, "   -bfr      Reverse order of bytes and bit-flip bits in file; uses a default extension of .bfr (may not be used with -r)\n");
    fprintf(stderr, "   -r      Reverse order of bytes in file without doing a bit-flip; uses default extension of .r (may not be used with -bfr)\n\n");
    exit(EXIT_SUCCESS);
}


//Function to check max size
void maxSizeCheck(FILE* inputFile, int maximumFileSize) {

    // Obtain and store size of file
	fseek(inputFile, 0L, SEEK_END);
	long int fileSize = ftell(inputFile);

    // Check if file size is greater than 25 kB (25,000 bytes)
    if (fileSize > 25000) {
        // Print error and exit program
        fprintf(stdout, "Error: The file is over 25 kB (file size was %ld)\n", fileSize);
        exit(EXIT_FAILURE);
    }
	// Check if file size is greater than maximum size
	if (fileSize > maximumFileSize) {
		// Print error and exit program
		fprintf(stdout, "Error: The file is over the maximum file size of %d (file size was %ld)\n", maximumFileSize, fileSize);
	    exit(EXIT_FAILURE);
	}

    // Rewind the pointer
	rewind(inputFile);

}


void bitFlip( char * inputFileName, FILE * inputFile, bool override, char * overrideFileName ) {
    
    fseek(inputFile, 0L, SEEK_END);

    long int fileSize = ftell(inputFile);

    fseek(inputFile, 0, SEEK_SET);
    
    // Initialize new file pointer
    FILE* newFile;
    
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

    // Initiali ze newFileName
    char * newFileName = NULL;

    // Check override variable to see if it is true
    if (override == true) {
        // Set new file name to the override file name
        newFileName = overrideFileName;
    }
    else {
        // Set new file name to original input file name
        newFileName = inputFileName;
    }

    // Use new file name and concatenate with .bf extension
    strcat(newFileName, ".bf");

    // Open new file to read
    newFile = fopen(newFileName, "r");

    // check if file existed already 
    if(newFile != NULL){
        printf("Error: File %s already exists!\n", newFileName);
        exit(EXIT_FAILURE);
    }
    
    // Open new file to write
    newFile = fopen(newFileName, "w");

    // Check if file couldn't be made
    if (newFile == NULL) {
        // Show error message and exit
        printf("Error: unable to create file\n");
        exit(EXIT_FAILURE);
    }
    
    // Loop through the entire file to replace the characters
	for (int charReplace = 0; charReplace < fileSize ; charReplace++) {
	
		// write the Bitflip in new file
        buffer[charReplace] ^= 0xFF;
        fwrite(&buffer[charReplace], sizeof(char), 1, newFile);
        
	} 
    
    // Close the new file
    fclose(newFile); 

}

void bitFlipReverse( char * inputFileName, FILE * inputFile, bool override, char * overrideFileName ) {

    fseek(inputFile, 0L, SEEK_END);

    long int fileSize = ftell(inputFile);

    fseek(inputFile, 0, SEEK_SET);
    
    // Initialize new file pointer
    FILE* newFile;
    
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

    // Initialize newFileName
    char * newFileName = NULL;

    // Check override variable to see if it is true
    if (override == true) {
        // Set new file name to the override file name
        newFileName = overrideFileName;
    }
    else {
        // Set new file name to original input file name
        newFileName = inputFileName;
    }


    // Use new file name and concatenate with .bf extension
    strcat(newFileName, ".bfr");

    // Open new file to read
    newFile = fopen(newFileName, "r");

    // check if file existed already 
    if(newFile != NULL){
        printf("Error: File %s already exists!\n", newFileName);
        exit(EXIT_FAILURE);
    }
    // Open new file to write
    newFile = fopen(newFileName, "w");

    // Check if file couldn't be made
    if (newFile == NULL) {
        // Show error message and exit
        printf("Error: unable to create file\n");
        exit(EXIT_FAILURE);
    }
    
    // Loop through the entire file to replace the characters
	for (int charReplace = fileSize - 1 ; charReplace  >= 0; charReplace--) {
	
		// write the Bitflip in new file
        buffer[charReplace] ^= 0xFF;
        fwrite(&buffer[charReplace], sizeof(char), 1, newFile);
        
	} 
    
    // Close the new file
    fclose(newFile); 

}


void reverse( char * inputFileName, FILE * inputFile, bool override, char * overrideFileName ) {
    fseek(inputFile, 0L, SEEK_END);

    long int fileSize = ftell(inputFile);

    fseek(inputFile, 0, SEEK_SET);
    
    // Initialize new file pointer
    FILE* newFile;
    
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

    // Initialize newFileName
    char * newFileName = NULL;

    // Check override variable to see if it is true
    if (override == true) {
        // Set new file name to the override file name
        newFileName = overrideFileName;
    }
    else {
        // Set new file name to original input file name
        newFileName = inputFileName;
    }

    // Use new file name and concatenate with .bf extension
    strcat(newFileName, ".r");

    // Open new file to read
    newFile = fopen(newFileName, "r");

    // check if file existed already 
    if(newFile != NULL){
        printf("Error: File %s already exists!\n", newFileName);
        exit(EXIT_FAILURE);
    }

    newFile = fopen(newFileName, "w");

    // Check if file couldn't be made
    if (newFile == NULL) {
        // Show error message and exit
        printf("Error: unable to create file\n");
        exit(EXIT_FAILURE);
    }
    
    // Loop through the entire file to replace the characters
	for (int charReplace = fileSize - 1 ; charReplace  >= 0; charReplace--) {
		// write reverse order in new file
        fwrite(&buffer[charReplace], sizeof(char), 1, newFile);
        
	} 
    
    // Close the new file
    fclose(newFile); 

}

/* Main Execution */
int main(int argc, char *argv[]) {

    // Set up Files
    char * inputFileName = NULL;
    char * overrideFileName = NULL;
 
    // Initialize argument index at 1
    int argIndex = 1;

    // Initialize file count to 0
    int fileCount = 0;

    int maximumFileSize = 25000;

    // Initialize booleans to check for functions
    char * flip = "normal";
    bool override = false;

    // Check if there is more than one argument
    if (argc < 2) {
        // Print error and exit program
	    fprintf(stdout, "Error: Please enter more than one argument\n");
	    exit(EXIT_FAILURE);
    }
    

    /* While loop to set up the different flag values depending on the user input */
    while (argIndex < argc) {

        // Find the file name
        if ((strchr(argv[argIndex], '.')) != NULL) {
            // Increase file count
            fileCount += 1;

            // Check if there are more than two files in arguments
            if (fileCount > 2) {
                // Show error message and exit
                fprintf(stdout, "Error: More than two files entered\n");
                exit(EXIT_FAILURE);
            }

            // Store the input file name
            inputFileName = argv[argIndex];

        }

        // Check if the length of the current argument index is greater than 1 and if it starts with - to see if it's a flag
        if (strlen(argv[argIndex]) > 1 && argv[argIndex][0] == '-') {
           
            // Initialize character pointer to current argument
            char *arg = argv[argIndex];
      
            // If and else if loops to check user flags and call respective functions
            if (strcmp(arg, "-help") == 0) {
                usage();
            }
            if (strcmp(arg, "-o") == 0) {

                // Check if there's no argument after override flag
                if (argv[argIndex + 1] == NULL) {
                    // Print error message and exit
                    fprintf(stdout, "Error: Please enter the desired new file name after -o\n");
                    exit(EXIT_FAILURE);
                }
                
                // Store the output file
                overrideFileName = argv[argIndex+1];

                // Use access function to check if file exists
                if (access(overrideFileName, F_OK) == 0) {
                    // Since file to be overwritten exists, show error message and exit
                    fprintf(stdout, "Error: An already existing file would be overwritten, please enter another file name\n");
                    exit(EXIT_FAILURE);
                }
                // *** MISSING: FPRINTF BELOW IS FOR ERROR CHECKING, REMOVE ONCE OUTPUTFILE IS USED !!! ***
                fprintf(stdout, "Override output file name: %s\n", overrideFileName);

                // Set override boolean to true
                override = true;
            } else if (strcmp(arg, "-maxsize") == 0) {
                // Check if there's no argument after maxsize flag
                if (argv[argIndex + 1] == NULL) {
                    // Print error message and exit
                    fprintf(stdout, "Error: Please enter the desired max file size after -maxsize\n");
                    exit(EXIT_FAILURE);
                }
                else if (argv[argIndex + 1] != NULL) {

                    // Get maximum size which is the argument after -maxsize flag
                    maximumFileSize = atoi(argv[argIndex + 1]);

                    // Check if maximum size is positive
                    if (maximumFileSize <= 0) {
                        // Print error message and exit
                        fprintf(stdout, "Error: The maximum file size entered must be positive\n");
                        exit(EXIT_FAILURE);
                    }
                }
            } else if (strcmp(arg, "-bfr") == 0) {

                // Update flip to bfr
                flip = "bfr";

            } else if (strcmp(arg, "-r") == 0) {

                // Update flip to r
                flip = "r";

            } else {
                // Show message to use -help argument and exit
                fprintf(stdout, "Wrong input, please enter -help as an argument to see the executable usage.\n");
                exit(EXIT_FAILURE);
            }
        
        }

        // Increase argument index by 1
        argIndex += 1;
    
    }

    // Create a file pointer and store it
    FILE* inputFile = fopen(inputFileName, "r");

    // Check if file exists
    if (inputFile == NULL) {
        // Print error and exit program
        fprintf(stdout, "Error: File does not exist\n");
        exit(EXIT_FAILURE);
    }

    // Check if file is readable
    if (access(inputFileName, R_OK ) == -1) {
        // Print error and exit program
        fprintf(stdout, "Error: File is not readable\n");
        exit(EXIT_FAILURE);
    }

    maxSizeCheck(inputFile, maximumFileSize);

    // Check if the file count is 0
    if (fileCount == 0) {
        // Show error message and exit
        fprintf(stdout, "Error: Please enter a file\n");
        exit(EXIT_FAILURE);
    }

    // Check if normalFlip boolean is true to call the bitflip function
    if ( strcmp(flip,"r") == 0 ){
        // Call specific function
        reverse(inputFileName, inputFile, override, overrideFileName);

    }else if( strcmp(flip,"rbf") == 0 ){
        // Call specific function
        bitFlipReverse(inputFileName, inputFile, override, overrideFileName);

    }else{
        bitFlip(inputFileName, inputFile, override, overrideFileName);
    }

    // Close the input file
    fclose(inputFile);

    // Return exit success
    return EXIT_SUCCESS;

}