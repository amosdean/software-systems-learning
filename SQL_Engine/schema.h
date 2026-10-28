#ifndef schema_H
#define schema_H
#include <stddef.h>
#include <stdio.h>

int schema();

typedef enum {
    DATA_TYPE_INTEGER,
    DATA_TYPE_TEXT
} DataType;

typedef struct {
    char name[32];
    DataType type;
} Column;

typedef struct {
    char name[32];
    Column *columns;
    size_t column_count;
} TableSchema;

FILE *openSchema(); 
int schemaAppend(char *table_name, Column *columns, size_t column_count);
int schema();

#endif // schema_H