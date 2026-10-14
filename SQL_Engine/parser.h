#ifndef parser_H
#define parser_H

typedef enum ParserState {
    START
    ,IN_SELECT
    ,IN_FROM
    ,IN_WHERE
} ParserState;

typedef enum ParserAction {
    TO_SELECT
    ,TO_FROM
    ,TO_WHERE
    ,PARSER_CONT
    ,PARSER_ERR
    ,PARSER_END
} ParserAction;

typedef struct TreeNode {
    Token *token;
    struct TreeNode *firstChild;
    struct TreeNode *nextSiblingNode;
} TreeNode;

#endif // parser_H