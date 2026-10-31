#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "schema.h"

int createTable(Statement *stmt) {
    // pull column information from the parse tree and pass it to schemaAppend
    TreeNode *node = stmt->rootNode;
    TreeNode *nextNode;
    char *tableName = node->firstChild->token->lexeme;
    printf("Creating table: %s\n", tableName);
    nextNode = node->nextSiblingNode->firstChild; // consume '('
    Column **columns = calloc(10, sizeof(Column *));
    int columnsIndex = 0;
    Column *column;
    
    while (nextNode != NULL) {
        node = nextNode;
        // reset column
        column = malloc(sizeof(Column));

        /* Column Name*/
        column->name = node->firstChild->token->lexeme;
        node = node->firstChild; // advance to type

        /* Column Type*/
        if(strcmp(node->token->lexeme, "INT"))
            column->type = DATA_TYPE_INTEGER;
        else
            column->type = DATA_TYPE_TEXT;

        /* Debug */
        printf("Column name: %s, Column Type: %s, index: %d\n", column->name, DATA_TYPES[column->type], columnsIndex);

        /* Add to columns*/
        columns[columnsIndex++] = column;

        // advance to next column        
        nextNode = node->firstChild; 
    }
    // printf("Exited column while loop\n"); // debug
    schemaAppend(tableName, columns, columnsIndex); // Placeholder for columns
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