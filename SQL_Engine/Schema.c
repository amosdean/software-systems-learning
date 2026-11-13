#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schema.h"
#include "lexer.h"

const int MAX_COLUMNS = 100;
// const int NUMBER_OF_DATATYPES = 2; // must match number of entries in DATA_TYPES array

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

// returns NULL table if no table of that name exists
TableSchema *getTable(char *tableName) {
    FILE *fp = openSchema();
    char line[256];
    char *table = "TABLE ";
    char *tableSearchTerm = malloc(strlen(table) + strlen(tableName) + 1);
    strcpy(tableSearchTerm, table);
    strcat(tableSearchTerm, tableName);
    TableSchema *ts; 
    // strcat(tableSearchTerm, "\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, tableSearchTerm)) {
            // found table, initialize schema
            ts = malloc(sizeof(TableSchema));
            ts->name = malloc(strlen(tableName) + 1);
            strcpy(ts->name, tableName);
            // parse columns
            ts->column_count = 0;
            ts->columns = calloc(sizeof(Column *), MAX_COLUMNS); 
            while (fgets(line, sizeof(line), fp) != NULL && strncmp(line, "TABLE ", 6) != 0) {
                Column *column = malloc(sizeof(Column));
                strtok(line, " "); // skip "COLUMN"
                char *name = strtok(NULL, " ");
                column->name = malloc(strlen(name) + 1);
                strcpy(column->name, name);
                char *type = strtok(NULL, "\n");
                DataType columnType = strtol(type, NULL, 10); // convert type to integer
                column->type = columnType;
                column->index = ts->column_count;
                ts->columns[ts->column_count++] = column;
            }
            fclose(fp);    
            return ts;
        }
    }
    fclose(fp);    
    return NULL;
}

FILE *openSchema() {
    FILE *file = fopen(".\\SQL_Engine\\schema.txt", "a+");
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