#ifndef schema_H
#define schema_H
#include <stddef.h>

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

#endif // schema_H