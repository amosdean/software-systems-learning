#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "commands.h"

int tokensLength;

char ** parseInput(char buffer[]) {
	char *token = strtok(buffer, " ");	
	char **tokens = malloc(sizeof(char*) * 10);
	int i;
	for (i = 0; token != NULL && i < 10; i++) {
		tokens[i] = token;
		token = strtok(NULL, " ");
	
	}
	tokens[i-1] = strtok(tokens[i-1], "\n"); // remove stray newline character
	tokensLength = i;
	return tokens;
}

void executeProgram(char *tokens[]) {
	pid_t pid = fork();
	int returnStatus;
	if (pid == 0) {
		execvp(tokens[0], tokens);	
		perror("execvp error");
		exit(1);
	}
	else {
		waitpid(pid, &returnStatus, 0);
	}
}

int commands(char **tokens) {
	char *keywords[] = {
		"cd"
		,"q"
		,"quit"
		,"pwd"
	};
	void (*functions[])(char **, int) = {
		cd
		,q
		,quit
		,pwd
	};
	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
		if (!strcmp(tokens[0], keywords[i])) {
			functions[i](tokens, tokensLength);
			return 1;
		}
	}
	// set variable
	if(strchr(tokens[0], '=') != NULL) {
		setVariable(tokens);
		return 1;
	}
	return 0;
}

int main(void) {
	char buffer[100];
	char **tokens;
	while(1) {
		char pwd[100];
		getcwd(pwd, sizeof(pwd));
		printf("\nShell>%s$ ", pwd);
		fgets(buffer, sizeof(buffer), stdin);

		tokens = parseInput(buffer);

		// run shell commands
		if(commands(tokens) == 1)
			continue;
		
		// expand variables
		for(int i = 1; i < tokensLength; i++) {
			if(tokens[i][0] == '$') {
				tokens[i] = getenv(tokens[i] + sizeof(char));
			}
		}

		// non-shell programs
		executeProgram(tokens);
	}
}
