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
#include <sys/types.h>
#include <ctype.h>

// Initialize PIDS and count
pid_t the_PIDS [100] = {0};
int pidCount = 0;

// Function to handle control-C gracefully
void handleControlC(int signal) {

    // Use wait status on the pid
    int pid = wait(NULL);

    // Check if process id is greater than 0
    if (pid > 0) {

      // Kill the process with the signal using the kill function
      kill(pid, signal);

      // Show message saying control-C was pressed and exit gracefully
      printf("\nndshell: process %d exited abnormally with signal %d: %s.\n",pid, WTERMSIG(signal), strsignal(WTERMSIG(signal))); 

      // Flush to standard out     
      fflush(stdout);

    }

}

// Function to start
void start_call(char *token[]) {

    // Initialize process id
    pid_t rc;

    // Use fork to create a new process
    rc = fork();

    // Check if rc is less than zero to check if fork fails
    if (rc < 0) {

        // Print out the error message and exit
        printf("ndshell: error forking the process...\n");
        return;

    } else if (rc == 0) { // Check if rc is zero to check child process

        // Check if the start in the execution is unsuccessful to print message and kill the process
        if (execvp(token[0], token) == -1){
            printf("ndshell: start unsuccesful. killing forked process...\n");
            kill(getpid(),9);
        }

    // Else to check if the rc is greater than 0 to check the parent process
    } else {

        // Print message that the process has started
        printf("ndshell: process %d started\n", rc);

        // Update the process id and count
        the_PIDS[pidCount] = rc;
        pidCount ++;

    }

}

// Function to wait
void wait_call(){

  // Initialize status
  int status;

  // Initialize old rc and call wait for the status
  int old_rc = wait(&status);

  // Check the old rc to determine exit status and show message
  if (old_rc < 0){
    printf("ndshell: No children\n");
  }else if (WIFEXITED(status)){
    printf("ndshell: process %d exited normally with status %d\n",old_rc,WEXITSTATUS(status));
  }else if (WIFSIGNALED(status)){ 
    printf("ndshell: process %d exited abnormally with signal %d: %s.\n",old_rc, WTERMSIG(status), strsignal(WTERMSIG(status)));
  }

}

// Function for waitfor call
void waitfor_call(int child){
  
  // Initialize status
  int status;

  // Initialize old rc and use waitpid function with child and status
  int old_rc = waitpid(child, &status, 0);

  // Check the old rc to determine exit status and show message
  if (old_rc < 0){
    printf("ndshell: no such process\n");
  }else if (WIFEXITED(status)){
    printf("ndshell: process %d exited normally with status %d\n",old_rc,WEXITSTATUS(status));
  }else if (WIFSIGNALED(status)){
    printf("ndshell: process %d exited abnormally with signal %d: %s.\n",old_rc, WTERMSIG(status), strsignal(WTERMSIG(status)));
  }

}

// Function to kill processes
void kill_call(int child) {

  // Initialize parent process using getpid
  int parent = getpid();

  // Check if the parent is the same as the child
  if (child == parent) {
    printf("ndshell: cannot kill parent process\n");
    return;
  }

  // Kill child and wait for it
  kill(child, 9);
  waitfor_call(child);

}

// This signal handler is used to tell bound that it should end
// because the command used in bound was unsuccesful
void sighandler() {
  return;
}

// Function to bound the call
void bound_call(char *token[]) {

  // Initialize char pointer for the arguments
  char *args[248];
  int i;

  // Loop through the arguments to get tokens
  for(i = 0; i < 248; i++ ){
    if(token[i+1]==NULL){
      args[i]=NULL;
      break;
    }
    args[i] = token[i+1];
  }

  // Initialize parent process using getpid and use fork for child
  int parent = getpid();
  int child = fork();

  // Check if child is less than 0
  if (child < 0) {
    printf("ndshell: error in bound command\n");
    return;
  }

  // Check if child is equal to 0
  if (child == 0) {

    // Check if start is unsuccessful to kill forked process and display messages
    if (execvp(args[0], args) == -1){
      printf("ndshell: start unsuccesful. command might not exist.\n");
      printf("ndshell: killing forked process...\n");
      kill(parent, SIGUSR1);
      kill(getpid(), 9);
    }
  }
  // Else to check if the process started
  else {
    printf("ndshell: process %d started\n", child);

    // Update process ids and count
    the_PIDS[pidCount] = child;
    pidCount ++;

    // Call signal function
    signal(SIGCHLD, sighandler);
    signal(SIGUSR1, sighandler);

    // Use sleep function with token
    int s = sleep(atoi(token[0]));

    // Kill child process
    kill(child, 9);

    // Initialize status and previous rc
    int status;
    int rc_prev = waitpid(child, &status, 0);

    // Check if the process exceeded the time limit or its exit status to display corresponding message
    if (s == 0) {
      printf("ndshell: process %d exceeded the time limit, killing it...\n", rc_prev);
    }
    if (WIFEXITED(status)){
      printf("ndshell: process %d exited normally with status %d\n",rc_prev,WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
      printf("ndshell: process %d exited abnormally with signal %d: %s.\n",rc_prev, WTERMSIG(status), strsignal(WTERMSIG(status)));
    }
  }

}

// Function to run call
void run_call(char *token[]){

  // Initialize rc with fork
  int rc = fork();

  // Check if rc is less than 0
  if(rc < 0){
    // Handle error in forking
    printf("Error in forking process...\n");
    return;
  }
  // Check if rc is equal to 0
  if(rc == 0){

    // Check if start is unsuccessful to kill forked process and display messages
    if (execvp(token[0], token) == -1){
      printf("ndshell: start unsuccesful. command might not exist.\n");
      printf("ndshell: killing forked process...\n");
      kill(getpid(),9);
    }
  }
  // Else to print that the process has started, update process ids and count and call waitfor function
  else{
    printf("ndshell: process %d started\n", rc);
    the_PIDS[pidCount] = rc;
    pidCount ++;
    waitfor_call(rc);
  }

}

// Function to quit
void quit_call(){
  int i;
  int status;
  
  // Go through processes to kill
  for(i=0; i<100; i++){
    if(waitpid(the_PIDS[i], &status, WNOHANG) == 0 ){
      kill_call(the_PIDS[i]);
    }
  }

  // Print message that all child processes are complete and exit successfully
  printf("All child processes complete – exiting the shell\n");
  exit(EXIT_SUCCESS);

}

// Main function
int main(int argc, char *argv[]) {
    
    while(1){

        printf("ndshell> "); // Shell command prompt
        fflush(stdout);
        char userInput[1024]; // Initialize user input string
        fgets(userInput, 1024, stdin);

        if(userInput == NULL){ //use fgets to store user input from stdin
          printf("Input was NULL, Exiting shell immediately\n"); // if userInput was NULL exit shell
          break;
        }

        // Keep track of words in userInput ... Break up the words
        char *token[250]; 
        int num_token = 0; // Initialize the count to 0
        token[num_token] = strtok(userInput, " \t\n");
        char *args[249]; //define arguments we will pass ignoring first entry

        // if blank line was entered, then accept new command
        if(token[num_token]==NULL){
          continue;
        }

        // While loop to function as long as token is not null
        while(token[num_token] !=  NULL && num_token < 250){

          num_token ++; // Set the token vector at the increasing indexes
          token[num_token] = strtok(0, " \t\n"); // Use the strtok function to separate the token
          args[num_token - 1] = token[num_token]; // create passing arguments char**
        }

        // Use the signal function to handle control-C
        signal(SIGINT, handleControlC);

        int i;
        int val_check = 0; // check if input for waitfor and kill is integer
 
        // Go Check the arguments to see which functions to call
        // if user_input is exit or quit, exit shell
        if (strcmp(token[0], "exit") == 0) {
          printf("Exiting shell immediately\n");
          break;
        }else if(strcmp(token[0], "quit") == 0){
          quit_call();
          break;
        }else if(strcmp(token[0], "start")==0){
          start_call(args);
        }else if(strcmp(token[0], "wait")==0){
          wait_call();
        }else if(strcmp(token[0], "waitfor")==0){
          for (i = 0; i < strlen(token[1]); i++) {
            if (isdigit(token[1][i]) == 0) {
              val_check = 1;
            }
          }
          if ( val_check != 0) {
            printf("ndshell: waitfor input was not an integer. \n  Usage: waitfor <process_id>\n");
          }
          else {
            waitfor_call(atoi(token[1]));
          }
        }else if(strcmp(token[0], "kill")==0){
          for (i = 0; i < strlen(token[1]); i++) {
            if (isdigit(token[1][i]) == 0) {
              val_check = 1;
            }
          }
            if (val_check == 0) {
              kill_call(atoi(token[1]));
            }else {
              printf("ndshell: kill input was not an integer. \n  Usage: kill <process_id>\n");
            }
        }else if(strcmp(token[0], "bound")==0){
          for (i = 0; i < strlen(token[1]); i++) {
            if (isdigit(token[1][i]) == 0) {
              val_check = 1;
            }
          }
          if (val_check == 0) {
            bound_call(args);
          }else {
            printf("ndshell: bound input was not an integer. \n  Usage: bound <wait_time> <executable>\n");
          }
        }else{
          printf("ndshell: unknown command: %s\n",token[0] );
          continue;
        }
        
    }

  return 0;
  
}