#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void cd(char *tokens[]) {
    char pwd[100];
    if(!strcmp("..", tokens[1])) {
        getcwd(pwd, sizeof(pwd));
        char *lastSlash = strrchr(pwd, '/');
        printf("%s",lastSlash);
        *lastSlash = '\0';
        chdir(pwd);
    }

}
void quit() {
    exit(0);
}
void q() {
    quit();
}