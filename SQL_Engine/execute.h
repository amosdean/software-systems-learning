#ifndef execute_H
#define execute_H
int execute(Statement **statements, int statement_count); 

typedef enum {
    VALUE_INT,
    VALUE_STRING
} ValueType;

typedef struct {
    ValueType type;

    union {
        int integer;
        char *string;
    } data;

} Value;

typedef struct {
    Value *values;
    size_t value_count;
} Row;

typedef struct {
    Row *rows;
    size_t rowCount;
} TableRead;

#endif // execute_H