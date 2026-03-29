#include "properties_parse.h"
#include <stdio.h>
typedef struct {
    char *port;
    char *docroot;
}Props;

Props p;
Props *props_get() {
    return &p;
}


int main(void){

    p.docroot = getPropertyValue("docroot","app.properties");
    p.port = getPropertyValue("port","app.properties");
    char *test = getPropertyValue("lamekey","app.properties");
    printf("%s\n",props_get()->docroot);
}