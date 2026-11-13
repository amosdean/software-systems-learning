#ifndef schema_H
#define schema_H
#include <stdio.h>
#include "lexer.h"

int schema();

typedef struct {
    char *name;
    DataType type;
    int index;
} Column;

typedef struct {
    char *name;
    Column **columns;
    size_t column_count;
} TableSchema;

FILE *openSchema(); 
int schemaAppend(char *table_name, Column **columns, size_t column_count);
TableSchema *getTable(char *tableName); 
int schema();

#endif // schema_H