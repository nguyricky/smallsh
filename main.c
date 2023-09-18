#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// Adapted Code from:
// (1) https://github.com/caoyucen/OSU_CS344/blob/master/Program3_smallsh/smallsh.c
// (2) https://brennan.io/2015/01/16/write-a-shell-in-c/
// Credit to the Author(s)

int foregroundFlag = 0;

void start_shell(){
    printf(": ");
    fflush(stdout); 
}

void get_status(int status){
    // if statement to determine if the child process
    // is terminated normally or by a signal
    if(WIFEXITED(status)){
        printf("Exit value %d\n", WEXITSTATUS(status));
        fflush(stdout);
    } 
    else{
        printf("Terminated by signal %d\n", status);
        fflush(stdout);
    }
}

void change_directory(char **arguments){

    // local char variable(s)
    char *directory;

    // if the user does not designate a directory
    // set the directory to home directory
    if(arguments[1] == NULL){
        directory = getenv("HOME");
        chdir(directory);
    }
    // if the user does designate a directory
    // set the directory to that
    else{
        directory = arguments[1];
    }

    // print case if not successfull
    if(chdir(directory) != 0){
        printf("Invalid directory: %s\n", directory);
    }

}

void sigtstop_handle_function(int signal){
    // if we're in the normal state
    // set the state to foreground-only
    if(foregroundFlag == 0){
        char *msg1 = "\nEntering foreground-only mode (& symbol is now ignored)\n: ";
        
        // set the foreground-only mode on
        foregroundFlag = 1;
        write(STDOUT_FILENO, msg1, strlen(msg1));
        fflush(stdout);
    }
    //
    else{
        char *msg2 = "\nExiting foreground-only mode\n: ";

        // set the foreground-only mode off
        foregroundFlag = 0;
        write(STDOUT_FILENO, msg2, strlen(msg2));
        fflush(stdout);
    }
}

/*void variable_expansion(char* t, int s){

}*/

int main(){
    // int varibles
    int file = -1;                      
    int status = 0;
    int zero = 0;
    int pid;

    // char variables
    char* input_file = NULL;  
    char* output_file = NULL;    
    char variable_expansion[2048];
    char user_commands[2048];
    char* arguments[512];
    char* token;

    // struct handler to handle ^C command
    struct sigaction signitCHandler = {{0}};
    signitCHandler.sa_handler = SIG_IGN;            
    signitCHandler.sa_flags = 0;                    
    sigfillset(&signitCHandler.sa_mask);
    signitCHandler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &signitCHandler, NULL);

    // struct handler to handle ^Z command
    struct sigaction signitZHandler = {{0}};
    signitZHandler.sa_handler = &sigtstop_handle_function;
    signitZHandler.sa_flags = 0;
    sigfillset(&(signitZHandler.sa_mask));
    signitZHandler.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &signitZHandler, NULL);
    
    while(1){
        int fg = 1;   
        int temp = 0;     

        // prints the shell
        start_shell();

        // grabs the user command(s)
        if(fgets(user_commands, 2048, stdin) == NULL){
            return 0;
        }

        // begin parsing the user command(s) for the shell
        token = strtok(user_commands, " \n");

        while(token != NULL){
            // if statements to handle file redirection
            // handles output file redirection
            if(strcmp(token, ">") == 0){    
                // parses the file and stores the information
                token = strtok(NULL, " \n");
                output_file = strdup(token);
                token = strtok(NULL, " \n");
            } 
            // handles input file redirection
            else if(strcmp(token, "<") == 0){
                // parses the file and stores the information
                token = strtok(NULL, " \n");
                input_file = strdup(token);
                token = strtok(NULL, " \n");
            } 
            // handles '&' symbol
            else if(strcmp(token, "&") == 0){
                fg = 0;
                break;
            } 
            // default handle
            else{
                arguments[temp] = strdup(token);
                token = strtok(NULL, " \n");
                temp++;
            }
        }
        // sets the arguments array to NULL
        arguments[temp] = NULL;        

        // checks for no input and comments
        // if found do nothing and reprompt
        if(arguments[0] == NULL || *(arguments[0]) == '#'){
            ;
        }
        // checks if the user wants to change directories
        // then handles the with the change_directory function
        else if(strcmp(arguments[0], "cd") == 0){
            change_directory(arguments);
        } 
        // checks if the user wants to check status
        // then prints the status
        else if(strcmp(arguments[0], "status") == 0){
            get_status(status);
        }
        // checks if the user wants to exit the program
        // then exits the program
        else if (strcmp(arguments[0], "exit") == 0){
            exit(0);
        } 
        else{
            // starts the child process(es)
            pid = fork();
            
            switch(pid){
                // if there is a error forking
                // trigger the error message
                case -1:    
                    printf("Hull Breach!\n");
                    status = 1;
                    break;
                // if there is no error forking
                case 0:
                    // if input file redirection was stated
                    // from the user
                    if(input_file != NULL){            
                        file = open(input_file, O_RDONLY);
                        // error message if there was
                        // a problem opening the file
                        if(file == -1){
                            printf("cannot open %s for input\n", input_file);
                            fflush(stdout);
                            exit(1);
                        }
                        // error message if there was
                        // a problem after redirection
                        if(dup2(file, 0) == -1){
                            perror("error in input redirection\n");
                            exit(1);
                        }
                        close(file);
                    } 
                    // if there is background mode is on
                    // and there isn't a file specified
                    else if(!fg){
                        // file redirects to /dev/null
                        file = open("/dev/null", O_RDONLY);
                        // error message if there was
                        // a problem opening the file
                        if(file == -1){
                            printf("cannot open %s for input\n", input_file);
                            fflush(stdout);
                            exit(1);
                        }
                        // error message if there was
                        // a problem after redirection
                        if(dup2(file, 0) == -1){
                            printf("error in input redirection");
                            fflush(stdout);
                            exit(1);
                        }
                    }
                    // if output file redirection was stated
                    // from the user
                    if(output_file != NULL){
                        file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        // error message if there was
                        // a problem opening the file
                        if(file == -1) {
                            printf("cannot open %s for output\n", output_file);
                            fflush(stdout);
                            exit(1);
                        }
                        // error message if there was
                        // a problem after redirection
                        if(dup2(file, 1) == -1){
                            printf("error in output redirection");
                            fflush(stdout);
                            exit(1);
                        }
                        close(file);
                    }
                    // if statement to print an error
                    // if user command was not found
                    if(execvp(arguments[0], arguments)){
                        printf("%s: no such command, file, or directory\n", arguments[0]);
                        fflush(stdout);
                        exit(1);
                    }
                    break;
                // instructions for the parent process
                default:    
                    // if process is a foreground-only process
                    if (fg){
                        waitpid(pid, &status, 0);
                    } 
                    // if process is a background process
                    else{                    
                        printf("background pid is %d\n", pid);
                        break;
                    }
            }

        }

        // clears the arguments for the next instance
        while(zero < temp){
            arguments[zero] = NULL;
            zero++;
        };
        
        // frees all the input and output files
        free(input_file);
        free(output_file);

        // sets all the input and output files
        // to NULL
        input_file = NULL;
        output_file = NULL;

        // check for the completed background
        // processes then prints them
        pid = waitpid(-1, &status, WNOHANG);

        while(pid > 0){
            printf("background pid %d, is complete: ", pid);
            get_status(status);
            pid = waitpid(-1, &status, WNOHANG);
        }
    }

    return 0;
}
