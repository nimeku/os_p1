#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <cstring>
#include <string>
#include <vector>

using namespace std;


bool isBackground(char* string){
    char* c;
    int index;
    
    if(c = strchr(string, '\0'))
        index = (int)(c - string);
    
    for(int i = (index-1); i>=0; i--){        
        if((string[i] == '&') && (string[i-1] == ' '))
            return true;
        
        if((string[i] != ' '))
            return false;
    }
    return false;
}
 


int main(int argc, char * argv[]){
    
    int childPid;
    int tempPid;
    int status;
    char cwd[256];
    char input[256];
    char inputCopy[256];
    char *command;
    char *parameters[60];
    bool background = false;
    bool normalState = true;
    
    while(1) {
        
        //return user-info and current working directory if not background
        getcwd(cwd, sizeof(cwd));
        printf("%s@miniShell:%s$ ", getpwuid(getuid())->pw_name, cwd);

        //read command and parameters
        fgets(input, 256, stdin);
        command = strchr(input, '\n');

        //replace/delete /n-char if found
        if(command){
            *command = '\0';
        }

        //exit exception
        if(strncmp(input, "exit",4)==0){
            exit(0);
        }

        // determine if background processing is requested
        // delete trailing &
        if(background = isBackground(input)){
            command = strchr(input, '&');
            *command = '\0';
            strncpy(inputCopy, input, 256);
        }

        // cd exception
        if(strncmp(input, "cd ",3)==0){
            strtok(input, " ");
            command = strtok(NULL, "\0");
            if(chdir(command) != 0){
                perror("Error while changing directory. Please try again!");
            }
            continue;
        }

        if ((childPid = fork()) == -1) {
            fprintf(stderr,"can't fork\n");
            exit(0);
        }else if (childPid == 0) { /* child process*/
            command = strtok(input, " ");
            parameters[0] = command;
            int i = 1;
            while((parameters[i] = strtok(NULL, " ")) != NULL){
                i++;
            }
            if(execvp(command, parameters) < 0){
                cout << command << ": command not found!" << endl;
            }
            exit(0);
        } else { /* parent process */
        if(background){
            tempPid = childPid;
            cout << "[" << childPid << "]" << endl;
            continue;
        }
        //wait(&status);

        int waited;
        if((waited = wait(&status)) == tempPid){
            cout << "Process " << waited << " done (" << inputCopy << ")" << endl;
            normalState = false;
        }

        } /* endif parent */
    } /* end while forever */
    return 0;
}