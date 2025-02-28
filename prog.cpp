#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

#define MAX_LINE 80  // The maximum length command
#define MAX_HISTORY 10

char *history[MAX_HISTORY];
int history_count = 0;

/**
 * @brief Parse out the command and arguments from the input command separated by spaces
 *
 * @param command
 * @param args
 * @return int
 */
int parse_command(char command[], char *args[]) {
    int count = 0;
    char *token = strtok(command, " \n");
    while (token != nullptr) {
        args[count++] = token;
        token = strtok(nullptr, " \n");
    }
    args[count] = nullptr;  // Null-terminate the array
    return count;
}

/**
 * @brief The main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The exit status of the program
 */
int main(int argc, char *argv[]) {
    char command[MAX_LINE];       // The command that was entered
    char *args[MAX_LINE / 2 + 1]; // Hold parsed out command line arguments
    int should_run = 1;           // Flag to determine when to exit program

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        // Read the input command
        if (fgets(command, MAX_LINE, stdin) == nullptr) {
            continue;
        }

        // Parse the input command
        int num_args = parse_command(command, args);

        //args check
        if (num_args == 0) {
            continue;
        }
                // Handle "!!" command properly
        if (strcmp(args[0], "!!") == 0) {
            if (history_count == 0) {
                printf("No commands in history.\n");
                continue;
            }
            // Retrieve last command from history
            strcpy(command, history[history_count - 1]);  
            printf("%s\n", command);  // Show the user the retrieved command

            // Remove trailing newline (just in case)
            command[strcspn(command, "\n")] = '\0';  

            // Re-parse the command
            num_args = parse_command(command, args);  

            // Skip adding "!!" to history and instead execute the last command
        }
        else {
            // Store the command in history
            if (history_count < MAX_HISTORY) {
                history[history_count] = strdup(command);
                history_count++;
            } else {
                free(history[0]);  // Free the oldest entry
                for (int i = 1; i < MAX_HISTORY; i++) {
                    history[i - 1] = history[i];
                }
                history[MAX_HISTORY - 1] = strdup(command);
            }
        }


        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            continue;
            
        } else if (pid == 0) {  // Child process
            int fd;
            for (int i = 0; i < num_args; i++) {
                if (strcmp(args[i], ">") == 0) { // Output Redirection
                    fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                    if (fd == -1) {
                        perror("Error opening file");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);  // Redirect standard output to file
                    close(fd);
                    args[i] = nullptr;  // Remove ">" from arguments
                    break;
                } else if (strcmp(args[i], "<") == 0) { // Input Redirection
                    fd = open(args[i + 1], O_RDONLY);
                    if (fd == -1) {
                        perror("Error opening file");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);  // Redirect standard input from file
                    close(fd);
                    args[i] = nullptr;  // Remove "<" from arguments
                    break;
                }
            }
            if (execvp(args[0], args) == -1) {
                perror("Error execution");
                exit(EXIT_FAILURE);
            }
        }

        // Parent handle background execution
        else {
            int background = (num_args > 0 && strcmp(args[num_args - 1], "&") == 0);
            if (background) {
                args[num_args - 1] = nullptr;  // Remove "&"
                num_args--;
            } else {
                wait(nullptr);
            }
        }
    }
    //test
    return 0;
}
