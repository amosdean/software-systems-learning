#include "schema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int schema()
{
    FILE *file = fopen("schema.txt", "rb");
    if (file == NULL) {
        perror("schema.txt");
        return 1;
    }

    char schema[4096];
    size_t length = fread(schema, 1, sizeof(schema) - 1, file);
    schema[length] = '\0';
    fclose(file);

    printf("Schema:\n%s\n", schema);

    return 0;
}