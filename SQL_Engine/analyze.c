#include <stdio.h>
#include "Parser.h"
#include "schema.h"

int analyzeInsert(Statement *stmt) {
    TreeNode *node = stmt->rootNode;
    node = node->nextNode; // Move to the  keyword
    node = node->nextNode; // Move to the table name
    char *tableName = node->token->lexeme;
    // verify table exists
    TableSchema *ts = getTable(tableName);
    if(ts != NULL) { 
        printf("Table \"%s\" already exists\n", tableName);
        return 0;
    }
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