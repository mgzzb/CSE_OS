/**********************************
 * Name: Andres Gonzalez and Mariana Gonzalez 
 * Email: agonza42@nd.edu mgonza32@nd.edu 
 * File Name: chime.c
 * Date Created: Feb 19, 2023
 * File Contents:
    The purpose of this milestone is to write threaded code whereby you will be prompting / reading 
    input in a primary thread and then having background threads acting as independent timers or 
    chimes. In contrast to previous milestones, you will be given a skeleton for the code (all milestones / 
    projects going forward will be similar) to modify to add functionality that includes: 
    • Modify the contents of a global struct and create a thread with an appropriate argument 
    • On exit, join all active threads and close gracefully  
 **********************************/

// Include required libraries
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Initialize keep looping variable and define the max number of threads
char g_bKeepLooping = 1;
#define MAX_THREADS 5

// Initialize struct for the chime thread information
struct ChimeThreadInfo
{
    int        nIndex;
    float      fChimeInterval;
    char       bIsValid;
    pthread_t  ThreadID;
};

// Instantiate the struct
struct ChimeThreadInfo  TheThreads[MAX_THREADS];

// Function for the thread chime data
void * ThreadChime (void * pData)
{
    struct ChimeThreadInfo  * pThreadInfo;
    /* Which chime are we? */
    pThreadInfo = (struct ChimeThreadInfo *) pData;
    while(g_bKeepLooping)
    {
        sleep(pThreadInfo->fChimeInterval);
        printf("Ding - Chime %d with an interval of %f s!\n", pThreadInfo->nIndex, pThreadInfo->fChimeInterval);
    }
    return NULL;
}

// Define the buffer size
#define BUFFER_SIZE 1024

// Function for main functionality of program
int main (int argc, char *argv[])
{
    
    // Allocate buffer size
    char szBuffer[BUFFER_SIZE];
    
    // Set all of the thread information to be invalid (none allocated)
    for (int j = 0; j < MAX_THREADS; j++) {
        TheThreads[j].bIsValid = 0;
    }
    
    while(1) {
        
        // Prompt and flush to stdout
        printf("CHIME>");
        fflush(stdout);

        // Wait for user input via fgets
        fgets(szBuffer, BUFFER_SIZE, stdin);

        // If the command is quit - join any active threads and finish up gracefully
        // Use strcmp function to see if user wants to exit
        if (strcmp(szBuffer, "exit\n") == 0) {
            
            // Stop the main loop
            g_bKeepLooping = 0;

            // Use a for loop to join all the valid threads 
            for (int currentIndex = 0; currentIndex < MAX_THREADS; currentIndex++) {

                // Check if the thread is valid
                if (TheThreads[currentIndex].bIsValid) {
                    
                    // Join the thread
                    pthread_join(TheThreads[currentIndex].ThreadID, NULL);

                    // Print out message that the thread is being joined
                    fprintf(stdout, "Joining Chime %d (Thread %lu)\n", currentIndex, (long unsigned) TheThreads[currentIndex].ThreadID);

                    // Print out message that the join is complete for the chime
                    fprintf(stdout, "Join Complete for Chime %d!\n", currentIndex);

                }

            }

            // Print message that the program is being exited and exit gracefully
            fprintf(stdout, "Exit chime program ...\n");
            exit(EXIT_SUCCESS);

        }

        // If the command is chime, the second argument is the chime number (integer) and the 
        // third number is the new interval (floating point). If necessary, start the thread
        // as needed
        // Use strncmp to see if user enters chime command
        if (strncmp(szBuffer, "chime", 5) == 0) {

            // Initialize index, interval, adjustTrue boolean
            int nIndex;
            float fInterval;
            bool adjustTrue = false;

            // Use sscanf function to parse the arguments from the input string
            int nParsed = sscanf(szBuffer, "chime %d %f\n", &nIndex, &fInterval);

            // Check if the user input makes sense and is correct
            if (nParsed == 2 && nIndex >= 0 && nIndex < MAX_THREADS && fInterval > 0.0f) {
                
                // If loop to check if the thread has already been allocated
                if (TheThreads[nIndex].bIsValid) {

                    // Cancel the existing thread before creating a new one if the thread already exists
                    pthread_cancel(TheThreads[nIndex].ThreadID);
                    pthread_join(TheThreads[nIndex].ThreadID, NULL);

                    // Print out message that the chime is being adjusted and update boolean to true
                    fprintf(stdout, "Adjusting chime %d to have an interval of %f s\n", nIndex, fInterval);
                    adjustTrue = true;

                }

                // Create the new thread and update its index, interval and isValid
                TheThreads[nIndex].nIndex = nIndex;
                TheThreads[nIndex].fChimeInterval = fInterval;
                TheThreads[nIndex].bIsValid = 1;
                int threadSuccess = pthread_create(&(TheThreads[nIndex].ThreadID), NULL, ThreadChime, &(TheThreads[nIndex]));

                // Check if thread was created successfully, if not, print out error message
                if (threadSuccess != 0) {
                    fprintf(stdout, "Error: Thread was not created successfully\n");
                }

                // Check if current chime has been adjusted, if not, print out message that a new thread is starting
                if (adjustTrue == false) {
                    fprintf(stdout, "Starting thread %lu for chime %d, interval of %f s\n", (unsigned long) TheThreads[nIndex].ThreadID, nIndex, fInterval);
                }

                // Update adjust boolean back to false
                adjustTrue = false;

            }

            // Else to see if chime is out of range and print out error message
            else if (nIndex < 0 || fInterval < 0) {
                printf("Cannot adjust chime %d, out of range\n", nIndex);
            }
            
        // Else to see if the command is unknown
        } else {
            printf("Unknown command: %s", szBuffer);
        }

    }
    
    return 0;

}