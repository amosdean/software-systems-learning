#ifndef schema_H
#define schema_H
#include <stdio.h>

int schema();

typedef enum {
    DATA_TYPE_INTEGER,
    DATA_TYPE_TEXT
} DataType;

static const char *DATA_TYPES[] = {
    "INT",
    "TEXT"
};

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