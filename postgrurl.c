/*

    C Code that implements a URL handler for Postgres extensions. This handler mimics java.net.URL class.

*/

#include <stdio.h>
#include "postgres.h"
#include <stdlib.h> 
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils/palloc.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include <err.h>
#include <string.h>

PG_MODULE_MAGIC;

/*
Types and Functions declaration
*/

// Types
struct postgrurl{
    char raw[512];
    char host[50];
    char file[100];
    char query[100];
    int port;
    int defaultPort;
}; // Already processed url

typedef struct postgrurl postgrurl;

/*
    Constructors
*/

//TODO: Implement function, still needs a helper method to split the url into parts.
postgrurl* URL1(char* spec){
    postgrurl *url;
    url = (postgrurl *) palloc(sizeof(postgrurl)+1);
    strcpy(url->raw, spec);
    return url;
}

//TODO: Implement function
postgrurl* ULR2(char* protocol, char* host, int port, char* file){
    postgrurl *url;
    url = (postgrurl *) palloc(sizeof(postgrurl)+1);
    return url;
}

//TODO: Implement function
postgrurl* URL3(char* protocol, char* host, char* file){
    postgrurl *url;
    url = (postgrurl *) palloc(sizeof(postgrurl)+1);
    return url;
}

//TODO: Implement function
postgrurl* URL4(postgrurl* context, char* spec){
    postgrurl *url;
    url = (postgrurl *) palloc(sizeof(postgrurl)+1);
    return url;
}


//Functions
Datum url_in(PG_FUNCTION_ARGS);
Datum url_out(PG_FUNCTION_ARGS);
Datum url_rcv(PG_FUNCTION_ARGS);
Datum url_send(PG_FUNCTION_ARGS);
Datum equals(PG_FUNCTION_ARGS);
Datum greater_than(PG_FUNCTION_ARGS);
Datum less_than(PG_FUNCTION_ARGS);
Datum greater_than_equals(PG_FUNCTION_ARGS);
Datum less_than_equals(PG_FUNCTION_ARGS);
Datum getAuthority(PG_FUNCTION_ARGS);

/*
Postgres type functions definition
*/

PG_FUNCTION_INFO_V1(url_in);
Datum url_in(PG_FUNCTION_ARGS){
    char *rawstr = PG_GETARG_CSTRING(0);
    postgrurl *url;
    url = URL1(rawstr);
    PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(url_out);
Datum url_out(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = (char *) palloc(256*sizeof(char));
    output = psprintf("%s", url->raw);
    PG_RETURN_CSTRING(output);
}

//TODO: Implement function
PG_FUNCTION_INFO_V1(url_rcv);
Datum url_rcv(PG_FUNCTION_ARGS){
    char *str = PG_GETARG_CSTRING(0);
    PG_RETURN_NULL();
}

//TODO: Implement function
PG_FUNCTION_INFO_V1(url_send);
Datum url_send(PG_FUNCTION_ARGS){
    char *str = PG_GETARG_CSTRING(0);
    PG_RETURN_NULL();
}

/*
    Methods and predicates
*/

PG_FUNCTION_INFO_V1(equals);
Datum equals(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = strcmp(url1->raw, url2->raw);
    if(eq==0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(greater_than);
Datum greater_than(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = strcmp(url1->raw, url2->raw);
    if(eq>0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(less_than);
Datum less_than(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = strcmp(url1->raw, url2->raw);
    if(eq<0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(greater_than_equals);
Datum greater_than_equals(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = strcmp(url1->raw, url2->raw);
    if(eq>=0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(less_than_equals);
Datum less_than_equals(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = strcmp(url1->raw, url2->raw);
    if(eq<=0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(getAuthority);
Datum getAuthority(PG_FUNCTION_ARGS){
    char *rawurl = (postgrurl *) PG_GETARG_CSTRING(0);
    postgrurl *url;
    url = URL1(rawurl);
    PG_RETURN_NULL();
}

static int getDefaultPort(postgrurl *url){
    return 0;
}

static char* getFile(postgrurl *url){
    return 'not implemented yet';
}

static char* getHost(postgrurl *url){
    return 'not implemented yet';
}

static char* getPath(postgrurl *url){
    return 'not implemented yet';
}

static int getPort(postgrurl *url){
    return 0;
}

static char* getProtocol(postgrurl *url){
    return 'not implemented yet';
}

static char* getQuery(postgrurl *url){
    return 'not implemented yet';
}

static char* getRef(postgrurl *url){
    return 'not implemented yet';
}

static char* getUserInfo(postgrurl *url){
    return 'not implemented yet';
}

static bool sameFile(postgrurl *url1, postgrurl *url2){
    return false;
}

static bool sameHost(postgrurl *url1, postgrurl *url2){
    return false;
}

static char* toString(postgrurl *url1){
    return 'not implemented yet';
}

