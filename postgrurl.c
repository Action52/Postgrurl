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
#include <regex.h>

PG_MODULE_MAGIC;

/*
Types and Functions declaration
*/

const int RAW_URL_MAX_SIZE = 230;
const int SCHEME_MAX_SIZE = 10;
const int HOST_MAX_SIZE = 50;
const int FILE_MAX_SIZE = 200;
const int QUERY_MAX_SIZE = 1000;

// Types
struct postgrurl{
    char *raw;
    char *scheme;
    char *host;
    char *file;
    char *query;
    int port;
    int defaultPort;
}; // Already processed url

typedef struct postgrurl postgrurl;

/*
    Constructors
*/

//Helper function to check if a given string url is valid
int comp_regex(char* rawurl, char* regexExpr, int returnValueIfSuccess){
    regex_t reegex;
    int value;
    value = regcomp(&reegex, regexExpr, 0);
    value = regexec(&reegex, regexExpr, 0, NULL, 0);
    if(value == 0){
        return returnValueIfSuccess;
    }else if (value==REG_NOMATCH){
        return -1;
    }
    else{
        return -2;
    }
}

//Under construction, please ignore
int check_valid_url(char* rawurl){
    char * expression1 = "\w*://\w*.\w/\w*";
    char * expression2 = "\w*://\w*.\w:\d{1,6}";
    int is_valid_expr_1 = comp_regex(rawurl, expression1, 1);
    int is_valid_expr_2 = comp_regex(rawurl, expression2, 2);
}

// Helper function to parse a raw url
postgrurl* url_from_str(char* rawurl){
    postgrurl* url = (postgrurl *) palloc(sizeof(postgrurl)+1);
    url->raw = strdup(rawurl);
    char *aux_url = strdup(rawurl);
    url->scheme = NULL;
    url->host = NULL;
    url->file = NULL;
    url->query = NULL;
    url->port = NULL;
    url->defaultPort = NULL;
    // First separate the scheme
    //char * scheme = (char *) palloc(SCHEME_MAX_SIZE*sizeof(char) + 1);
    char *token = strtok(aux_url, "://");
    if(strcmp(token, url->raw) != 0){ // This means that the URL actually contained a scheme.
        url->scheme = strdup(token);
        aux_url = strtok(NULL, "://");
    }
    char *token2 = strtok(aux_url, "?q=");
    while(token2 != NULL){
        token2 = strtok(NULL, "?q=");
    }
    if (token2 != NULL){
        url->query = strdup(token2);
    }
    return url;
}

//TODO: Implement function, still needs a helper method to split the url into parts.
postgrurl* URL1(char* spec){
    postgrurl *url = url_from_str(spec);
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
    postgrurl *url = URL1(rawstr);
    PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(url_out);
Datum url_out(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = psprintf(
        "raw: %s, scheme: %s, host: %s, file: %s, query: %s, port: %d, defaultPort: %d", 
        url->raw, url->scheme, url->host, url->file, url->query, url->port, url->defaultPort);
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

