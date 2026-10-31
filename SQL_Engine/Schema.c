#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schema.h"

int schemaAppend(char *table_name, Column **columns, size_t column_count) {
    FILE *file = openSchema();
    if (!file) return 1;

    // Move to the end of the file
    fseek(file, 0, SEEK_END);

    // Write table name
    fprintf(file, "TABLE %s\n", table_name);
    // Write columns
    for (size_t i = 0; i < column_count; i++) {
        fprintf(file, "COLUMN %s %d\n", columns[i]->name, columns[i]->type);
    }
    fclose(file);
    return 0;
}

FILE *openSchema() {
    FILE *file = fopen(".\\SQL_Engine\\schema.txt", "a");
    if (file == NULL) {
        perror("schema.txt");
        return NULL;
    }
    return file;
}

int schema()
{
    FILE *file = fopen("schema.txt", "rb");
    if (file == NULL) {
        perror("schema.txt");
        return 1;
    }

    char schema[4096];
    size_t length = fread(schema, 1, sizeof(schema) - 1, file);
    schema[length] = '\0';
    fclose(file);

    printf("Schema:\n%s\n", schema);

    return 0;
}