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


// Function to handle control-C gracefully
void handleControlC(int signal) {
 
    // Show message saying control-C was pressed and exit gracefully
    fprintf(stdout, "\nControl-C was pressed ... exiting\n");
    exit(EXIT_SUCCESS);

}

void start_call(char *token[]){
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
        if (execvp(token[0], token) == -1){
            printf("ndshell: start unsuccesful. killing forked process...\n");
            kill(getpid(),9);
        }
    } else {  // Check if rc is greater than 0 to check the parent process
        printf("ndshell: process %d started\n", rc);
    }
}

void wait_call(){
  int status;
  int old_rc = wait(&status);
  if (old_rc < 0){
    printf("ndshell: No children\n");
  }else if (WIFEXITED(status)){
    printf("ndshell: process %d exited normally with status %d\n",old_rc,WEXITSTATUS(status));
  }else if (WIFSIGNALED(status)){ 
    printf("ndshell: process %d exited abnormally with signal %d: %s.\n",old_rc, WTERMSIG(status), strsignal(WTERMSIG(status)));
  }
}

void waitfor_call(int child){
  int status;
  int old_rc = waitpid(child, &status, 0);

  if (old_rc < 0){
    printf("ndshell: no such process\n");
  }else if (WIFEXITED(status)){
    printf("ndshell: process %d exited normally with status %d\n",old_rc,WEXITSTATUS(status));
  }else if (WIFSIGNALED(status)){
    printf("ndshell: process %d exited abnormally with signal %d: %s.\n",old_rc, WTERMSIG(status), strsignal(WTERMSIG(status)));
  }
}

void kill_call(int child) {
  int parent = getpid();
  if (child == parent) {
    printf("ndshell: cannot kill parent process\n");
    return;
  }
  kill(child, 9);
  waitfor_call(child);
}

// This signal handler is used to tell bound that it should end
// because the command used in bound was unsuccesful
void sighandler() {
  return;
}

void bound_call(char *token[]) {
  char *args[248];
  int i;
  for(i = 0; i < 248; i++ ){
    if(token[i+1]==NULL){
      args[i]=NULL;
      break;
    }
    args[i] = token[i+1];
  }
  int parent = getpid();
  int child = fork();
  if (child < 0) {
    printf("ndshell: error in bound command\n");
    return;
  }
  if (child == 0) {
    if (execvp(args[0], args) == -1){
      printf("ndshell: start unsuccesful. command might not exist.\n");
      printf("ndshell: killing forked process...\n");
      kill(parent, SIGUSR1);
      kill(getpid(), 9);
    }
  }
  else {
    printf("ndshell: process %d started\n", child);
    signal(SIGCHLD, sighandler);
    signal(SIGUSR1, sighandler);
    int s = sleep(atoi(token[0]));
    kill(child, 9);
    int status;
    int rc_prev = waitpid(child, &status, 0);
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

void run_call(char *token[]){
  int rc = fork();
  if(rc<0){
    // handle error in forking
    printf("Error in forking process...\n");
    return;
  }
  if(rc==0){
    if (execvp(token[0], token) == -1){
      printf("ndshell: start unsuccesful. command might not exist.\n");
      printf("ndshell: killing forked process...\n");
      kill(getpid(),9);
    }
  }
  else{
    printf("ndshell: process %d started\n", rc);
    waitfor_call(rc);
  }
}

void quit_call(){
  printf("All child processes complete – exiting the shell.");
}


// Main function
int main(int argc, char *argv[]) {
    
    while(1){

        printf("ndshell>"); // Shell command prompt
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