
/**
 * Assignment 2: Simple UNIX Shell
 * @file pcbtable.h
 * @author Joel Burlingame (TODO: your name)
 * @brief This is the main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @version 0.1
 */
// You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient and clear comments to your code

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

#define MAX_LINE 80 // The maximum length command

/**
 * @brief parse out the command and arguments from the input command separated by spaces
 *
 * @param command
 * @param args
 * @return int
 */
int parse_command(char command[], char *args[])
{
    int count = 0;
    char *token = strtok(command, " \n");
    while(token != nullptr){
        args[count++] = token;
    }
    args[count] = nullptr;
    return count;
}

// TODO: Add additional functions if you need

/**
 * @brief The main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The exit status of the program
 */
int main(int argc, char *argv[])
{
    char command[MAX_LINE];       // the command that was entered
    char *args[MAX_LINE / 2 + 1]; // hold parsed out command line arguments
    int should_run = 1;           /* flag to determine when to exit program */

    // TODO: Add additional variables for the implementation.

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);
        // Read the input command
        if(fgets(command, MAX_LINE, stdin) == nullptr){
            continue;
        }
        // Parse the input command
        int num_args = parse_command(command, args);

        if(num_args == 0){
            continue;
        }
        if(strcmp(args[0], "exit")){
            should_run == 0;
            continue;
        }
        pid_t pid = fork();

        if(pid < 0){
            perror("Fork failded");
            continue;
        }
        else if(pid == 0){
            if(execvp(args[0], args) == -1){
                perror("Error execution");
            }
            exit(EXIT_FAILURE);
        }
        else{
            int background = (num_args > 0 && strcmp(args[num_args - 1], "&") == 0);
            if(!background){
                wait(nullptr);
            }
        }
    }
    return 0;
}