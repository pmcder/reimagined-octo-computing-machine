#ifndef PROPERTIES_PARSE_H
#define PROPERTIES_PARSE_H
#include <stdio.h>

struct Field {
    char *key;
    char *value;
};

struct Object {
    struct Field *fields;
    size_t count;
};

const char *getPropertyValue(char *key, char *filename);

#endif
