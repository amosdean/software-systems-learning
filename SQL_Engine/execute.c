#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "schema.h"
#include "table.h"

DataType TranslateDataType(Token *token) {
    switch (token->type) {
        case TOK_NUMBER:
            return DATA_TYPE_INTEGER;
        case TOK_IDENTIFIER:
            return DATA_TYPE_TEXT;
        default:
            return -1; // Invalid type
    }
}

int createTable(Statement *stmt) {
    // pull column information from the parse tree and pass it to schemaAppend
    TreeNode *node = stmt->rootNode;
    TreeNode *nextNode;
    char *tableName = node->childNode->token->lexeme;

    TableSchema *ts = getTable(tableName);
    if(ts != NULL) { 
        printf("Table \"%s\" already exists\n", tableName);
        return 0;
    }

    printf("Creating table: %s\n", tableName);
    nextNode = node->nextNode->childNode; // consume '('
    Column **columns = calloc(10, sizeof(Column *));
    int columnsIndex = 0;
    Column *column;
    
    while (nextNode != NULL) {
        node = nextNode;
        /* reset column */
        column = malloc(sizeof(Column));

        /* Column Name */
        column->name = node->childNode->token->lexeme;
        node = node->childNode; // advance to type

        /* Column Type */
        if(strcmp(node->token->lexeme, "INT"))
            column->type = DATA_TYPE_INTEGER;
        else
            column->type = DATA_TYPE_TEXT;

        /* Debug */
        printf("Column name: %s, Column Type: %s, index: %d\n", column->name, DATA_TYPES[column->type], columnsIndex);

        /* Add to columns*/
        columns[columnsIndex++] = column;

        /* advance to next column */ 
        nextNode = node->childNode; 
    }
    // printf("Exited column while loop\n"); // debug
    schemaAppend(tableName, columns, columnsIndex); // Placeholder for columns
    return 1; 
}

int executeInsert(Statement *stmt) {
    TreeNode *node = stmt->rootNode;
    node = node->nextNode; // Move to the insert keyword
    TreeNode *tableNode = node->childNode; // Move to the table name
    char *tableName = tableNode->token->lexeme;

    /* Analyze the INSERT statement */
    // verify table exists
    TableSchema *ts = getTable(tableName);
    if(ts == NULL) { 
        printf("Table \"%s\" does not exist\n", tableName);
        return 0;
    }
    
    // verify columns match
    node = node->nextNode; // values node 
    node = node->childNode; // '(' node
    TreeNode *row = node;
    char *columnsToInsert[100][100] = {0}; // Assuming a maximum of 100 columns and 100 rows for simplicity
    size_t rowIndex = 0;
    // node = node->childNode; // Move to the '('
    while(node->childNode != NULL) { // multiple rows
        for (size_t i = 0; i < ts->column_count; i++) {
            if (ts->columns[i]->type != TranslateDataType(node->token)) {
                printf("Syntax Error: Expected value (identifier or number) on line %d\n", node->token->line);
                return 0;
            }
            columnsToInsert[rowIndex][i] = malloc(strlen(node->token->lexeme) + 1);
            strcpy(columnsToInsert[rowIndex][i], node->token->lexeme);
            node = node->childNode; // Move to the next value
            if (node == NULL) {
                printf("Syntax Error: Mismatched number of values for table \"%s\"\n", tableName);
                return 0;
            }
            node = node->childNode; 
        }
        rowIndex++;
        node = node->childNode; // Move to the next row
    }
    /* Execute INSERT */
    char *filePath = malloc(strlen(".\\SQL_Engine\\") + strlen(tableName) + strlen(".tbl") + 1);
    sprintf(filePath, ".\\SQL_Engine\\%s.tbl", tableName);
    FILE *file = fopen(filePath, "a+");
    if (file == NULL) {
        // create the file if it doesn't exist
        file = fopen(filePath, "w");
        if (file == NULL) {
            printf("Error accessing table file for \"%s\"\n", tableName);
            return 0;
        }
    }
    for (size_t i = 0; i < rowIndex; i++) {
        for (size_t j = 0; j < ts->column_count; j++) {
            fprintf(file, "%s", columnsToInsert[i][j]);
            if (j < ts->column_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
    free(filePath);
    return 1;
}

int execute(Statement **statements, int statement_count) {
    for (int i = 0; i < statement_count; i++) {
        Statement *stmt = statements[i];
        switch (stmt->type) {
            case ST_SELECT:
                printf("Executing SELECT statement\n");
                break;
            case ST_INSERT:
                printf("Executing INSERT statement\n");
                break;
            case ST_CREATE:
                printf("Executing CREATE TABLE statement\n");
                createTable(stmt);
                break;
            default:
                printf("Unknown statement type\n");
                break;
        }
    }
    return 1; // Indicate success
}