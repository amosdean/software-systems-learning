#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "commands.h"
#include "variables.h"

// int pid = -1;
// int preforked = 0;
// int tokensLength;
// void file_pipe()

char ** parseInput(char buffer[]) {
	char *token = strtok(buffer, " ");	
	char **tokens = malloc(sizeof(char*) * 10);
	int i;
	for (i = 0; token != NULL && i < 10; i++) {
		tokens[i] = token;
		token = strtok(NULL, " ");
	}
	tokens[i-1] = strtok(tokens[i-1], "\n"); // remove stray newline character
	tokens[i] = NULL;
	return tokens;
}

void redirect(char *file, char io, int append) {
	int fDes;
	if(io == 'I') fDes = open(file, O_RDONLY);
	else fDes = open(file, O_WRONLY | O_CREAT | (append == 1 ? O_APPEND : O_TRUNC), 0644); 

	if(fDes == -1) {
		printf("ERROR: non-existent file.");
		quit();
	};

	dup2(fDes, io == 'I' ? STDIN_FILENO : STDOUT_FILENO);
	close(fDes);
}

void executeProgram(char *tokens[], int preforked) {
	int pid;
	if(preforked == 0)
		pid = fork();
	int returnStatus;

	if (preforked != 0 || pid == 0) {
		// check for redirection
		for(int i = 0; tokens[i] != NULL; i++) {
			if(tokens[i][0] == '>' || tokens[i][0] == '<') {
				redirect(tokens[i+1], 
					tokens[i][0] == '<' ? 'I':'O',
					tokens[i][1] == '>' ? 1:0);
				tokens[i] = NULL;
				tokens[i+1] = NULL;
				break;
			}
		}
		execvp(tokens[0], tokens);	
		perror("execvp error");
		exit(1);
	}
	else {
		waitpid(pid, &returnStatus, 0);
	}
}

void arraySplitter(char *array[], int splitIndex, char **left[], char **right[]) {
	// int arraySize = sizeof(array) / sizeof(array[0]);
	int arraySize = 0;
	while (array[arraySize] != NULL) {
		arraySize++;
	}
	int rightSize = arraySize - splitIndex - 1; // minus 1, skipping |
	*left = malloc((splitIndex) * sizeof(array[0]));
	*right = malloc(rightSize * sizeof(array[0]));
	for(int i = 0; i < splitIndex; i++) {
		(*left)[i] = array[i];
	}
	(*left)[splitIndex] = NULL;
	for(int i = splitIndex + 1; i < arraySize; i++) {
		(*right)[i - splitIndex - 1] = array[i];
	}
	(*right)[rightSize] = NULL;
}

int commands(char **tokens) {
	char *keywords[] = {
		"cd"
		,"q"
		,"quit"
		,"pwd"
		,"extern"
	};
	void (*functions[])(char **) = {
		cd
		,q
		,quit
		,pwd
		,setExternalVariable
	};
	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
		if (!strcmp(tokens[0], keywords[i])) {
			functions[i](tokens);
			return 1;
		}
	}
	// set variable
	if(strchr(tokens[0], '=') != NULL) {
		setVariable(tokens[0]);
		return 1;
	}
	return 0;
}

char ** pipeline(char *tokens[]) {
		// ls | grep ".c"	
	int fD[2];
	char **left = NULL;
	char **right = NULL;
	int pid_one = NULL;
	int pid_two = NULL;
	int returnStatus;
	for(int i = 0; tokens[i] != NULL; i++) {
		if(tokens[i][0] == '|') {
			if(pipe(fD) == -1) {
				printf("ERROR: Could not open pipe");
				quit();
			}
			arraySplitter(tokens, i, &left, &right);
			pid_one = fork();
			// preforked = 1;
			if(pid_one == 0) {
				close(fD[0]);
				dup2(fD[1], STDOUT_FILENO);	
				executeProgram(left, 1);
			}
			else {
				pid_two = fork();
				if(pid_two == 0) {
					close(fD[1]);
					dup2(fD[0], STDIN_FILENO);
					executeProgram(right, 1);
				}
				// return tokens; // do not return - could be more pipes
			}
		}
	}
	close(fD[0]);
	close(fD[1]);
	waitpid(pid_one, &returnStatus, 0);
	waitpid(pid_two, &returnStatus, 0);
	return pid_one == NULL ? tokens : NULL;
}

void tokenHandler(char **tokens) {
		// run shell commands
		if(commands(tokens) == 1)
			return;
		
		// expand variables
		for(int i = 1; tokens[i] != NULL; i++) {
			if(tokens[i][0] == '$') {
				tokens[i] = getVariableValue(tokens[i]);//getenv(tokens[i] + sizeof(char));
			}
		}

		tokens = pipeline(tokens);

		if(tokens != NULL)
			// non-shell programs
			executeProgram(tokens, 0);
}

int main(void) {
	char buffer[100];
	char **tokens;
	char pwd[100];
	while(1) {
		// shell prompt
		getcwd(pwd, sizeof(pwd));
		printf("\nShell>%s$ ", pwd);
		fgets(buffer, sizeof(buffer), stdin);

		// parse
		tokens = parseInput(buffer);

		tokenHandler(tokens);	
	}
	return 0;
}
