#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "schema.h"

char * readFile(const char *file, size_t *out_len) {
	FILE *fp = fopen(file, "rb");
	if(!fp) return NULL;

	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	if(sz < 0) {fclose(fp); return NULL;}

	char *buf = malloc((size_t)sz + 1);
	if(!buf) {fclose(fp); return NULL;}

	rewind(fp);
	size_t nread = fread(buf, 1, (size_t)sz, fp);
	fclose(fp);
	
	*out_len = nread;
	buf[*out_len] = '\0';
	return buf;

}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		printf("Invalid number of arguments");
		return 1;
	}
	size_t file_str_len = 0;
	char * buffer = readFile(argv[1], &file_str_len);
	printf("%s", buffer);
	// lexer
	int *tokens_length = malloc(sizeof(int));
	*tokens_length = 0;
	Token **tokens = scanTokens(buffer, file_str_len, tokens_length);
	// for(int i = 0; i <)
	// Parser
	parseTokens(tokens, tokens_length);
	// Schema
	schema();
	// Execute
		
	
	return 0;
}
