#include "parser.h"
#include "lexer.h"
#include "schema.h"

int createTable(Statement *stmt) {
    printf("Creating table: %s\n", stmt->rootNode->token->lexeme);
    // pull column information from the parse tree and pass it to schemaAppend
    TreeNode *node = stmt->rootNode->nextSiblingNode; 
    printf("token: %s\n", node->token->lexeme);
    // while (token) {
    //     printf("Column: %s\n", token->lexeme);
    //     token = token->nextSiblingNode;
    // }
    // schemaAppend(stmt->rootNode->token->lexeme, NULL, 0); // Placeholder for columns
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
                break;
            default:
                printf("Unknown statement type\n");
                break;
        }
    }
    return 1; // Indicate success
}