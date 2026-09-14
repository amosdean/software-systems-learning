#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void cd(char *tokens[]) {
    char pwd[100];
    // go to parent directory
    if(!strcmp("..", tokens[1])) {
        getcwd(pwd, sizeof(pwd));
        char *lastSlash = strrchr(pwd, '/');
        *lastSlash = '\0';
        chdir(pwd);
    }

    // to absolute directory
    if(tokens[1][0] == '/') {
        // getcwd(pwd, sizeof(pwd));
        // char *lastSlash = strrchr(pwd, '/');
        // *lastSlash = '\0';
        chdir(tokens[1]);
    }

    // to relative directory
    if(tokens[1][0] == '.') {
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
}
void quit() {
    exit(0);
}
void q() {
    quit();
}