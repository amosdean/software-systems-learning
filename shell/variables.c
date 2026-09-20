#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

struct Variable {
    char *key;
    char *value;
};

#define VARS_LENGTH 100
static int curSize = 0;
static struct Variable variables[VARS_LENGTH];


void setVariable(char *tokens) {
   if(curSize == VARS_LENGTH) {
    printf("ERROR: Max variables count reached.");
    return;
   }
   char *key = strtok(tokens, "=");  
   char *val = strtok(NULL, "\0");  
   variables[curSize].key = malloc(strlen(key) + 1);
   variables[curSize].value = malloc(strlen(val) + 1);
   
   strcpy(variables[curSize].key, key);
   strcpy(variables[curSize].value, val);
   curSize++;
}
 
char * getVariableValue(char *varName) {
    varName = varName + 1;
    for(int i = 0; i < curSize; i++) {
        if(!strcmp(varName,variables[i].key)){
            return variables[i].value;
        }
    }
    return NULL;
} 
