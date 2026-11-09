#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"
#include "schema.h"
#include "lexer.h"


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

int analyzeInsert(Statement *stmt) {
    TreeNode *node = stmt->rootNode;
    node = node->nextNode; // Move to the insert keyword
    TreeNode *tableNode = node->childNode; // Move to the table name
    char *tableName = tableNode->token->lexeme;

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
    // while(node->childNode != NULL) {
    //     node = node->childNode; // Move to the first column name
    //     char *columnName = node->token->lexeme;
    //     int columnExists = 0;
    //     for(size_t i = 0; i < ts->column_count; i++) {
    //         if(strcmp(ts->columns[i].name, columnName) == 0) {
    //             columnExists = 1;
    //             break;
    //         }
    //     }
    //     if(!columnExists) {
    //         printf("Column \"%s\" does not exist in table \"%s\"\n", columnName, tableName);
    //         return 0;
    //     }
    // }
    // node = columnsNode->nextNode; // Move to the '(' for values
    // while(node->childNode != NULL) {
    //     node = node->childNode; // Move to the first value
    //     char *value = node->token->lexeme;
    // }
    return 1;
}

int analyze(Statement **statements, int statement_count) {
    for (int i = 0; i < statement_count; i++) {
        Statement *stmt = statements[i];
        switch (stmt->type) {
            case ST_SELECT:
                printf("Analyzing SELECT statement\n");
                break;
            case ST_INSERT:
                printf("Analyzing INSERT statement\n");
                analyzeInsert(stmt);
                break;
            case ST_CREATE:
                printf("Analyzing CREATE TABLE statement\n");
                break;
            default:
                printf("Unknown statement type\n");
                break;
        }
    }
    return 1; // Indicate success
}