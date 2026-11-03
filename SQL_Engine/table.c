#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schema.h"


FILE *openTableFile(char *filename, char *mode) {
    FILE *file = fopen(strcat(".\\SQL_Engine\\Data\\", filename), mode);
    if (file == NULL) {
        perror(filename);
        return NULL;
    }
    return file;
}

int createTableFile(char * tableName) {
    FILE *file = openTableFile(tableName, "a+");
    if (!file) return 1;
}