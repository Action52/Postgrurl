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
#include <assert.h>

PG_MODULE_MAGIC;

/*
Types and Functions declaration
*/

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


//Check if a string only contains digits (for recognizing the port)
int digits_only(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

/*
    Constructors
*/

//Constructor with protocol, host, port, file, query
postgrurl* new_URL1(char* protocol, char* host,int port,char* file, char* query)
{
    char raw[1000];
    postgrurl* url = palloc(sizeof(postgrurl));
    url->scheme = strdup(protocol);
    url->host = strdup(host);
	url->file = strdup(file);
	url->port = port;
	url->query = strdup(query);

    snprintf(raw, sizeof(raw), "%s://%s:%d/%s?%s",
    protocol, host,port,file,query);
    url->raw = strdup(raw);

    return url;
}

//Constructor with protocol, host, port, file
postgrurl* new_URL2(char* protocol, char* host, int port, char* file) {
    char raw[1000];
	postgrurl* url = palloc(sizeof(postgrurl));
    url->scheme = strdup(protocol);
    url->host = strdup(host);
	url->file = strdup(file);
	url->port = port;

    snprintf(raw, sizeof(raw), "%s://%s:%d%s", protocol, host,port,file);
    url->raw = strdup(raw);

    return url;
}

//Constructor with protocol, host,file
postgrurl* new_URL3(char* protocol, char* host,char* file) {
    char raw[1000];
    postgrurl* url = palloc(sizeof(postgrurl));
    url->scheme = strdup(protocol);
    url->host = strdup(host);
	url->file = strdup(file);

    snprintf(raw, sizeof(raw), "%s://%s%s", protocol, host, file);
    url->raw = strdup(raw);

    return url;
}

//Constructor with protocol, host,port
postgrurl* new_URL4(char* protocol, char* host,int port) {
    char raw[1000];
    postgrurl* url = palloc(sizeof(postgrurl));
    url->scheme = strdup(protocol);
    url->host = strdup(host);
	url->port = port;

    snprintf(raw, sizeof(raw), "%s://%s:%d", protocol, host, port);
    url->raw = strdup(raw);

    return url;
}


//Constructor with protocol, host
postgrurl* new_URL5(char* protocol, char* host) {
  char raw[1000];
  postgrurl* url = palloc(sizeof(postgrurl));
  url->scheme = strdup(protocol);
  url->host = strdup(host);

  snprintf(raw, sizeof(raw), "%s:/%s", protocol, host);
  url->raw = strdup(raw);

  return url;
}

char * url_to_string(postgrurl* url){
    char port[5];
    char defaultPort[5];
    char * stringed_url = palloc(256*sizeof(char));
    strcpy(stringed_url, "(");
    if(url->raw != NULL){
        strcpy(stringed_url, "raw: ");
        strcat(stringed_url, url->raw);
    }
    if(url->scheme != NULL){
        strcat(stringed_url, ", scheme: ");
        strcat(stringed_url, url->scheme);
    }
    if(url->host != NULL){
        strcat(stringed_url, ", host: ");
        strcat(stringed_url, url->host);
    }
    if(url->file != NULL){
        strcat(stringed_url, ", file: ");
        strcat(stringed_url, url->file);
    }
    if(url->port != NULL){
        strcat(stringed_url, ", port: ");
        sprintf(port, "%d", url->port);
        strcat(stringed_url, port);
    }
    if(url->defaultPort != NULL){
        strcat(stringed_url, ", defaultPort:");
        sprintf(port, "%d", url->defaultPort);
        strcat(stringed_url, defaultPort);
    }
    if(url->query != NULL){
        strcat(stringed_url, ", query:");
        strcat(stringed_url, url->query);
    }
    return stringed_url;
}


//Parse a string to url data type
postgrurl* string_to_url(char* str){

	postgrurl* url;
	int ind = 0;
	char delim[] = "://";

	char *value;
	char *query_split;

	char scheme[50]="";
    char host[200]="";
    char file[500]="";
    char query[200]="";
	int port = 0;


	// Split string by delimiters "://"
	char *ptr = strtok(str, delim);

	while(ptr != NULL)
	{
		value = ptr;
		printf("%s\n", value);


		switch(ind){
			//first part should always be the protocol
			case 0:
				strcpy(scheme,value);
				break;
			//second should always be the host
			case 1:
				strcpy(host,value);
				break;
			//check if the third part has a port number
			case 2:
				if (digits_only(value) == 1){
					port = atoi(value);
					break;
				}

			default:
				//Check if remaining part in the file or if it contains
        //a "?" than it has a query part
				if(strstr(value,"?") != NULL)
				{
					query_split = strtok(value, "?");
					strcat(file,"/");
					strcat(file,query_split);

					//get the query part
					query_split = strtok(NULL, "?");
					strcat(query,"?");
					strcat(query,query_split);


				}else{
					strcat(file,"/");
					strcat(file,value);

				}
				break;
		}


		ptr = strtok(NULL, delim);
		ind++;
	}
	//call matching constructor
	//protocol,host,port,file, query
	if(port!=0 && strcmp(query,"")!=0 && strcmp(file,"")!=0)
	{
		url = new_URL1(scheme, host,port,file,query);
	}
	//protocol,host,port,file
	else if(port!=0 && strcmp(query,"")==0 && strcmp(file,"")!=0)
	{
		url = new_URL2(scheme, host,port,file);
	}
	//protocol,host,file
	else if(port==0 && strcmp(query,"")==0 && strcmp(file,"")!=0)
	{
		url = new_URL3(scheme,host,file);
	}
	//protocol,host,port
	else if(port!=0 && strcmp(query,"")==0 && strcmp(file,"")==0){
		url = new_URL4(scheme, host, port);
	}
	//protocol,host
	else if(port==0 && strcmp(query,"")==0 && strcmp(file,"")==0)
	{
		url = new_URL5(scheme, host);
	}
    strtok(NULL, NULL);
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
Datum getFile(PG_FUNCTION_ARGS);
Datum getHost(PG_FUNCTION_ARGS);
Datum getPort(PG_FUNCTION_ARGS);
Datum getProtocol(PG_FUNCTION_ARGS);
Datum getQuery(PG_FUNCTION_ARGS);
Datum getRef(PG_FUNCTION_ARGS);

/*
Postgres type functions definition
*/

PG_FUNCTION_INFO_V1(url_in);
Datum url_in(PG_FUNCTION_ARGS){
    char *rawstr = PG_GETARG_CSTRING(0);
    postgrurl *url = string_to_url(rawstr);
    PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(url_out);
Datum url_out(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = url_to_string(url);
    output = psprintf("%s", output);
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
    url = string_to_url(rawurl);
    PG_RETURN_NULL();
}

// // static int getDefaultPort(postgrurl *url){
// PG_FUNCTION_INFO_V1(getDefaultPort);
// Datum getDefaultPort(PG_FUNCTION_ARGS){
//     return 0;
// }

PG_FUNCTION_INFO_V1(getFile);
Datum getFile(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output = palloc(50*sizeof(char));
    
    if(url->file != NULL){
        strcat(output, url->file);
    }
    else{
        ereport(ERROR,
            (
             errmsg("no file in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}
    
PG_FUNCTION_INFO_V1(getHost);
Datum getHost(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output = palloc(50*sizeof(char));

    if(url->file != NULL){
        strcat(output, url->host);
    }
    else{
        ereport(ERROR,
            (
             errmsg("no host in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}

// PG_FUNCTION_INFO_V1(getPath);
// Datum getPath(PG_FUNCTION_ARGS){
// // static char* getPath(postgrurl *url){
    // everything after host (after the first /)
//     return 'not implemented yet';
// }

PG_FUNCTION_INFO_V1(getPort);
Datum getPort(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    int output;

    if(url->port != NULL){
        output = url->port;
    }
    else{
        ereport(ERROR,
            (
             errmsg("no port in the url")
            )
        );
    }
    output = psprintf("%d", output);
    PG_RETURN_INT32(output);
    pfree(output);
}

PG_FUNCTION_INFO_V1(getProtocol);
Datum getProtocol(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output = palloc(50*sizeof(char));

    if(url->scheme != NULL){
        strcat(output, url->scheme);
    }
    else{
        ereport(ERROR,
            (
             errmsg("no protocol in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}

PG_FUNCTION_INFO_V1(getQuery);
Datum getQuery(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output = palloc(50*sizeof(char));

    if(url->query != NULL){
        strcat(output, url->query);
    }
    else{
        ereport(ERROR,
            (
             errmsg("no query in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}

PG_FUNCTION_INFO_V1(getRef);
Datum getRef(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output = palloc(50*sizeof(char));
    char delim[] = "#";

    output = strtok(url->raw, delim); //
    output = strtok(NULL, delim);
    if (output == NULL) {
        ereport(ERROR,
            (
             errmsg("no reference in the url")
            )
        );
    }
    else{
        printf("%s", output);
    }

    PG_RETURN_CSTRING(output);
    pfree(output);
}

PG_FUNCTION_INFO_V1(getUserInfo);
Datum getUserInfo(PG_FUNCTION_ARGS){
// static char* getUserInfo(postgrurl *url){
    return 'not implemented yet';
}

PG_FUNCTION_INFO_V1(sameFile);
Datum sameFile(PG_FUNCTION_ARGS){
// static bool sameFile(postgrurl *url1, postgrurl *url2){
    return false;
}

PG_FUNCTION_INFO_V1(sameHost);
Datum sameHost(PG_FUNCTION_ARGS){
// static bool sameHost(postgrurl *url1, postgrurl *url2){
    return false;
}

PG_FUNCTION_INFO_V1(toString);
Datum toString(PG_FUNCTION_ARGS){
// static char* toString(postgrurl *url1){
    return 'not implemented yet';
}
