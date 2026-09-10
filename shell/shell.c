#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

char ** parseInput(char buffer[]) {
	char *token = strtok(buffer, " ");	
	char **tokens = malloc(sizeof(char*) * 10);
	int i = 0;
	for (int i = 0; token != NULL && i < 10; i++) {
		tokens[i] = token;
		token = strtok(NULL, " ");
	}
	return tokens;
}

void executeCommand(char *tokens[]) {
	pid_t pid = fork();
	int returnStatus;
	if (pid == 0) {
		execvp(tokens[0], tokens);	
		perror("execvp");
		exit(1);
	}
	else {
		waitpid(pid, &returnStatus, 0);
		printf("father");
	}

}

int main(int argc, char *argv[]) {
	char buffer[100];
	printf("\nShell>");
	fgets(buffer, sizeof(buffer), stdin);

	char **tokens = parseInput(buffer);
	executeCommand(tokens);
	// printf("\n%s", buffer);
	// printf("\n%s", tokens[0]);
}
