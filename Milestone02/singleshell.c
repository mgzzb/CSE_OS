/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: singleshell.c
 * Date Created: Feb 6, 2023
 * File Contents:
    Create an executable named singleshell that prompts the user for input and then attempts 
    to execute that user input (executable and arguments) inside of a child process.  Your code should 
    also capture Control-C and exit gracefully rather than defaulting to the normal system invocation 
    of exit.  Your code should appropriately reflect what was executed as well as demarcate when 
    execution is complete.  Once the execution is complete, your code should then appropriately 
    execute (hence the name singleshell).    
 **********************************/

// Include required libraries
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

    // Initialize boolean to track when the execution is done
    bool execDone = false;

    // Use the signal function to handle control-C
    signal(SIGINT, handleControlC);

    /*

    *** DO WE DO THE HANDLING OF CONTROL-C AS WE DID ABOVE, OR SOMETHING
        LIKE PROF.STRIEGEL DID IN CLASS AS SHOWN BELOW? ***

    struct sigaction new_action, old_action;
    //Set up the structure to specify the new action.
    new_action.sa_handler = termination_handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
    {
        sigaction (SIGINT, &new_action, NULL);
    }

    */

    // While loop runs as long as the execution isn't done
    while (execDone != true) {

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

            // Print when the execution is complete
            fprintf(stdout, "Execution complete!\n");

            // Change the execution done boolean to true
            execDone = true;

        }

    }

    // Exit successfully
    return EXIT_SUCCESS;

}