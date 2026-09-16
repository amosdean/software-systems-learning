#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void cd(char *tokens[], int tokensLength) {
    char pwd[100];

    // go to home directory
    if(tokensLength == 1) {
        char *home = getenv("HOME");
        chdir(home);
    }

    else if(tokensLength > 2) {
        printf("ERROR: Too many arguments\n");
    }

    // go to parent directory
    else if(!strcmp("..", tokens[1])) {
        getcwd(pwd, sizeof(pwd));
        char *lastSlash = strrchr(pwd, '/');
        *lastSlash = '\0';
        chdir(pwd);
    }

    // to absolute directory
    else if(tokens[1][0] == '/') {
        if(chdir(tokens[1]) == -1) {
            printf("ERROR: Invalid path\n");
        }
    }

    // to relative directory
    else if(tokens[1][0] == '.') {
        if(sizeof(tokens[1]) / (sizeof(tokens[1][0]) > 1) && tokens[1][1] == '.') {
            getcwd(pwd, sizeof(pwd));
            char temp[100];
            int i;
            for(i = 1; tokens[1][i] != '\0'; i++) {
                temp[i-1] = tokens[1][i];
            }
            temp[i] = '\0';
            strncat(pwd, temp, 100);
            chdir(pwd);
        }
        // else case stay in current directory
    }

}

void pwd() {
    char pwd[100];
    getcwd(pwd, sizeof(pwd));
    printf("%s", pwd);
}
void setVariable(char *tokens[]) {
    char *var = strtok(tokens[0], "=");
    char *val = strtok(NULL, "\0");
    setenv(var, val, 1); 
}

void quit() {
    exit(0);
}
void q() {
    quit();
}