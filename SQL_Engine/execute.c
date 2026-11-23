#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "schema.h"
#include "execute.h"

static const char *TABLE_DIR = ".\\SQL_Engine\\DB\\Tables\\";

DataType TranslateDataType(Token *token) {
    switch (token->type) {
        case TOK_NUMBER:
            return DATA_TYPE_INTEGER;
        case TOK_IDENTIFIER:
            return DATA_TYPE_VARCHAR;
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
        column->name = node->token->lexeme;
        node = node->childNode; // advance to type

        /* Column Type */
        if(!strcmp(node->token->lexeme, "INT"))
            column->type = DATA_TYPE_INTEGER;
        else
            column->type = DATA_TYPE_VARCHAR;

        /* Debug */
        printf("Column name: %s, Column Type: %s, index: %d\n", column->name, DATA_TYPES[column->type], columnsIndex);

        /* Add to columns*/
        columns[columnsIndex++] = column;

        /* advance to next column */ 
        nextNode = node->childNode; 
    }
    // printf("Exited column while loop\n"); // debug
    schemaAppend(tableName, columns, columnsIndex); // Placeholder for columns

    /* Memory Cleanup */ 
    for (int i = 0; i < columnsIndex; i++)
        free(columns[i]);
    free(columns);
    free(ts);

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
    while(node != NULL) { // multiple rows
        for (size_t i = 0; i < ts->column_count; i++) {
            if (ts->columns[i]->type != TranslateDataType(node->token)) {
                TranslateDataType(node->token);
                printf("Syntax Error: Expected value (identifier or number) on line %d\n", node->token->line);
                return 0;
            }
            columnsToInsert[rowIndex][i] = malloc(strlen(node->token->lexeme) + 1);
            strcpy(columnsToInsert[rowIndex][i], node->token->lexeme);
            node = node->childNode; // Move to the next value
            if (node == NULL && i < ts->column_count - 1) {
                printf("Syntax Error: Mismatched number of values for table \"%s\"\n", tableName);
                return 0;
            }
        }
        rowIndex++;
    }

    /* Execute INSERT */
    char *filePath = malloc(strlen(TABLE_DIR) + strlen(tableName) + strlen(".tbl") + 1);
    sprintf(filePath, "%s%s.tbl", TABLE_DIR, tableName);
    FILE *file = fopen(filePath, "a+");
    if (file == NULL) {
        // create the file if it doesn't exist
        file = fopen(filePath, "w");
        if (file == NULL) {
            printf("Error accessing table file for \"%s\"\n", tableName);
            free(filePath);
            free(ts);
            return 0;
        }
    }
    for (size_t i = 0; i < rowIndex; i++) {
        for (size_t j = 0; j < ts->column_count; j++) {
            fprintf(file, "%s", columnsToInsert[i][j]);
            if (j < ts->column_count - 1) 
                fprintf(file, "|");
        }
        fprintf(file, "\n");
    }
    fclose(file);

    /* Memory Cleanup */
    free(filePath);
    for (size_t i = 0; i < rowIndex; i++) {
        for (size_t j = 0; j < ts->column_count; j++) {
            free(columnsToInsert[i][j]);
        }
    }
    free(ts);
    return 1;
}

TableRead *readTableData(char *tableName) {
    TableSchema *ts = getTable(tableName);
    if(ts == NULL) {
        printf("Error: Table \"%s\" not found\n", tableName);
        free(ts);
        return NULL;
    }
    char *filePath = malloc(strlen(TABLE_DIR) + strlen(tableName) + strlen(".tbl") + 1);
    sprintf(filePath, "%s%s.tbl", TABLE_DIR, tableName);
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Error: Could not open table file for \"%s\"\n", tableName);
        free(filePath);
        return NULL;
    }

    char line[1024];
    TableRead *tableRead = malloc(sizeof(TableRead));
    tableRead->rowCount = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0'; // remove newline
        // split the line by '|' and store in tableRead->rows
        if(tableRead->rowCount == 0) {
            tableRead->rows = malloc(sizeof(Row));
        } else {
            tableRead->rows = realloc(tableRead->rows, (tableRead->rowCount + 1) * sizeof(Row));
        }
        Row *row = &tableRead->rows[tableRead->rowCount];
        row->values = malloc(ts->column_count * sizeof(Value));
        char *token = strtok(line, "|");
        size_t columnIndex = 0;
        while (token != NULL && columnIndex < ts->column_count) {
            if (ts->columns[columnIndex]->type == DATA_TYPE_INTEGER) {
                row->values[columnIndex].type = VALUE_INT;
                row->values[columnIndex].data.integer = atoi(token);
            } else {
                row->values[columnIndex].type = VALUE_STRING;
                row->values[columnIndex].data.string = malloc(strlen(token) + 1);
                strcpy(row->values[columnIndex].data.string, token);
                
            }
            token = strtok(NULL, "|");
            columnIndex++;
        }
        tableRead->rowCount++;
    }
    // free memory
    fclose(file);
    free(filePath);
    return tableRead;
}

int* analyzeSelect(Statement *stmt) {
    TreeNode *node = stmt->rootNode;
    node = node->nextNode->childNode; // Move to the table name
    char *tableName = node->token->lexeme;

    TableSchema *ts = getTable(tableName);
    if(ts == NULL) {
        printf("Error: Table \"%s\" not found\n", tableName);
        return NULL;
    }

    int *columnsToSelect = malloc(ts->column_count * sizeof(int));
    for (int i = 0; i < ts->column_count; i++)
        columnsToSelect[i] = -1; // Initialize with -1 to indicate not selected
    int selectedColumnIndex = 0;
    node = stmt->rootNode->childNode;
    while (node != NULL) {
        char *columnName = node->token->lexeme;
        int columnFound = 0;
        for (size_t i = 0; i < ts->column_count; i++) {
            if (strcmp(ts->columns[i]->name, columnName) == 0) {
                columnFound = 1;
                columnsToSelect[selectedColumnIndex++] = i; // Store the index of the column
                break;
            }
        }
        if (!columnFound) {
            printf("Error: Column \"%s\" not found in table \"%s\"\n", columnName, tableName);
            free(ts);
            return NULL;
        }
        node = node->childNode; // Move to the next column
    }
    
    free(ts);
    return columnsToSelect; // Indicate success
}

int executeSelect(Statement *stmt) {
    TreeNode *selectNode = stmt->rootNode;
    // selectNode = selectNode->nextNode; 
    int *columnsSelected = analyzeSelect(stmt);
    TableRead *tableRead = readTableData(selectNode->nextNode->childNode->token->lexeme);
    TableSchema *ts = getTable(selectNode->nextNode->childNode->token->lexeme);
    if(tableRead == NULL) {
        return 0;
    }
    // Print the selected columns
    for (size_t i = 0; i < tableRead->rowCount; i++) {
        Row *row = &tableRead->rows[i];
        for(size_t selectColumnIndex = 0; selectColumnIndex < sizeof(columnsSelected) / sizeof(int); selectColumnIndex++) {
            int columnIndex = columnsSelected[selectColumnIndex];
            if (columnIndex == -1) 
                break; // No more columns to select
            if (selectColumnIndex > 0)
                printf(" | ");
            if (row->values[columnIndex].type == VALUE_INT)
                printf("%d", row->values[columnIndex].data.integer);
            else
                printf("%s", row->values[columnIndex].data.string);
        }
        printf("\n");
    }

    // Free allocated memory
    free(columnsSelected);
    // freeTableRead(tableRead);
    free(ts);

    return 1;
}

int execute(Statement **statements, int statement_count) {
    for (int i = 0; i < statement_count; i++) {
        Statement *stmt = statements[i];
        switch (stmt->type) {
            case ST_SELECT:
                printf("Executing SELECT statement\n");
                executeSelect(stmt);
                break;
            case ST_INSERT:
                printf("Executing INSERT statement\n");
                executeInsert(stmt);
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