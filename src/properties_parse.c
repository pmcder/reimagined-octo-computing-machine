#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "properties_parse.h"

int parsePropFile(const char *filename, struct Object *obj){

    FILE *fileptr;
    fileptr = fopen(filename,"r");
    char message_str[100];
    char *key;
    char *value;
    struct Field field;

    /* fields is heap-allocated (not a fixed-size stack array) so it has no
     * hard cap on the number of keys and so obj->fields stays valid after
     * this function returns instead of pointing at freed stack memory. */
    size_t capacity = 8;
    size_t fldsize = 0;
    struct Field *fields = malloc(capacity * sizeof(struct Field));
    if (fields == NULL){
        return -1;
    }

    if (fileptr==NULL){
        free(fields);
        return -1;
    }
    else{
        int fieldCnt = 0;

        while (fgets(message_str, sizeof message_str, fileptr) != NULL) {
                int index = 0;
                size_t len = strlen(message_str);
                size_t i;
                    key  = malloc(len+1);
                    value  = malloc(len+1);
                    if (key == NULL || value == NULL){
                        free(key);
                        free(value);
                        free(fields);
                        fclose(fileptr);
                        return -1;
                    }
                for (i = 0; i < len; i++ ){
                char c = message_str[i];
                    if (c=='#') {
                        break;
                    }
                if (c != '='){
                    key[i]= c;
                }
                if (c == '=') {
                    index = i+1;
                    break;
                }
                }
                key[i] = '\0';

                int x;
                int v = 0;
                for (x = index; x < len; x++ ){

                char c = message_str[x];
                    if (c=='#') {
                        break;
                    }
                    if (c && c != '\n') {
                        value[v] = c;
                    }

                    v++;
                }
                value[v]= '\0';
                field.key = strdup(key);
                field.value = strdup(value);
                free(key);
                free(value);

                if ((size_t)fieldCnt == capacity){
                    capacity *= 2;
                    struct Field *grown = realloc(fields, capacity * sizeof(struct Field));
                    if (grown == NULL){
                        free(field.key);
                        free(field.value);
                        free(fields);
                        fclose(fileptr);
                        return -1;
                    }
                    fields = grown;
                }
                fields[fieldCnt] = field;
                fieldCnt++;
                fldsize++;
        }
    }
    obj->fields = fields;
    obj->count = fldsize;
    fclose(fileptr);
    return 1;
}

const char *getValue(const char *thiskey, struct Object *obj){
    for (size_t i = 0; i < obj->count;i++ ){
        if (strcmp(obj->fields[i].key,thiskey)==0){
            return obj->fields[i].value;
        }
    }
    return "key not found";
}

const char *getPropertyValue(char *key, char *filename) {
    struct Object obj;
    struct Object *ojb_ptr = &obj;
    if (parsePropFile(filename, ojb_ptr) < 0) {
        return NULL;
    }
    return  getValue(key,ojb_ptr);
}