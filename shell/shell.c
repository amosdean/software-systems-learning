#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

char ** parseInput(char buffer[]) {
	char *token = strtok(buffer, " ");	
	char **tokens = malloc(sizeof(char*) * 10);
	// while(token != NULL) {
	// }
	int i;
	for (i = 0; token != NULL && i < 10; i++) {
		tokens[i] = token;
		token = strtok(NULL, " ");
	
	}
	tokens[i-1] = strtok(tokens[i-1], "\n");
	return tokens;
}

void executeCommand(char *tokens[]) {
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

int main(void) {
	char buffer[100];
	char **tokens;
	while(1) {

		printf("\nShell>");
		fgets(buffer, sizeof(buffer), stdin);
		if(*buffer == 'q' || !strcmp(buffer, "quit") || !strcmp(buffer, "exit") ) return 0;
		tokens = parseInput(buffer);

		executeCommand(tokens);
	}
	// printf("\n%s", buffer);
	// printf("\n%s", tokens[0]);
}
