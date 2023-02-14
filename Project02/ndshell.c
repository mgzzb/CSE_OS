/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: ndshell.c
 * Date Created: Feb 12, 2023
 * File Contents:
    You will write a program called ndshell which is capable of executing, managing, and 
    monitoring user level programs. This program will be similar in purpose and design to everyday 
    shells like bash or tcsh, although the syntax will be slightly different.  ndshell will be 
    invoked without any arguments, and will support several different commands that include: 
 **********************************/

// Include required libraries
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

// Function to handle control-C gracefully
void handleControlC(int signal) {

    // Show message saying control-C was pressed and exit gracefully
    fprintf(stdout, "\nControl-C was pressed .. exiting\n");
    exit(EXIT_SUCCESS);

}

// Main function
int main(int argc, char *argv[]) {

    // Initialize user input string
    char userInput[1024];

    // Initialize process id
    pid_t rc;

    // Initialize the status for the process
    int status;

    // Use the signal function to handle control-C
    signal(SIGINT, handleControlC);

    // Print message asking for execute and use fgets to store user input from stdin
    fprintf(stdout, "Execute? ");
    fgets(userInput, 1024, stdin);

    // Use strcspn function to remove the newline character from the user input
    userInput[strcspn(userInput, "\n")] = 0;

    // Use fork to create a new process
    rc = fork();

    // Check if rc is less than zero to check if fork fails
    if (rc < 0) {

        // Print out the error message and exit
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);

    }

    // Check if rc is zero to check child process
    else if (rc == 0) {
        
        // Initialize a char pointer to argv
        char *argv[512];

        // Initialize the argument count to 0
        int argc = 0;

        // Initialize a char pointer for the token
        char *token;

        // Use the strtok function to separate the user input by spaces
        token = strtok(userInput, " ");

        // While loop to function as long as token is not null
        while (token != NULL) {
            
            // Set the argument vector at the increasing indexes equal to the token
            argv[argc++] = token;

            // Use the strtok function to separate the token
            token = strtok(NULL, " ");

        }

        // Set the argument vector at the current argc index to null
        argv[argc] = NULL;

        // Print the command that's being executed
        fprintf(stdout, "Executing: %s\n", userInput);

        // Execute the commands using the execvp function
        execvp(argv[0], argv);

        // Use perror function to check if an error occurs with execvp and exit if that's the case
        perror("Error");
        exit(EXIT_FAILURE);

    }
    
    // Check if rc is greater than 0 to check the parent process
    else {

        // Wait for the status
        wait(&status);

        if(status != EXIT_FAILURE){
            // Print when the execution is complete
            fprintf(stdout, "Execution complete!\n");
        }else{
            printf("Execution failed\n");
        }

    }

    // Exit successfully
    return EXIT_SUCCESS;

}