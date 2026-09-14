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

int parsePropFile(const char *filename, struct Object *obj);
const char *getValue(const char *thiskey, struct Object *obj);
const char *getPropertyValue(const char *key, const char *filename);

#endif
