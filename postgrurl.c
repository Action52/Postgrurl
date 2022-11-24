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

//get the default port for a protocol
int assignDefaultPort(const char *protocol){

	int defaultPort;

	if (strcmp(protocol, "http") == 0)
	{
		defaultPort = 80;

	}else if (strcmp(protocol, "https") == 0)
	{
		defaultPort = 443;

	}else
	{
		defaultPort = 0;
	}

	return defaultPort;
}

/*
    Constructors
*/

//Constructor with protocol, host, port, file
postgrurl* new_URL1(char* protocol, char* host, int port, char* file) {
	   postgrurl* url = palloc(sizeof(postgrurl));
     char *raw = malloc(sizeof(char) * ((strlen(protocol)+strlen(host)+strlen(file)) +6));

     url->scheme = strdup(protocol);
     url->host = strdup(host);
	   url->file = strdup(file);
	   url->port = port;

     url->query = NULL;

    snprintf(raw, sizeof(raw), "%s://%s:%d/%s", protocol, host,port,file);
    url->raw = strdup(raw);

    return url;
}

//Constructor with protocol, host,file
postgrurl* new_URL2(char* protocol, char* host,char* file) {
    postgrurl* url = palloc(sizeof(postgrurl));
    char *raw = malloc(sizeof(char) * ((strlen(protocol)+strlen(host)+strlen(file)) +6));
    url->scheme = strdup(protocol);
    url->host = strdup(host);
	  url->file = strdup(file);
    url->defaultPort = assignDefaultPort(protocol);

    url->query = NULL;
    url->port = 0;

    snprintf(raw, sizeof(raw), "%s://%s/%s", protocol, host, file);
    url->raw = strdup(raw);

    return url;
}


char * url_to_string(postgrurl* url){
    char port[5];
    char defaultPort[5];
    char * stringed_url = palloc(1024*sizeof(char));
    strcpy(stringed_url, "(");

    if(url->raw){
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
        sprintf(defaultPort, "%d", url->defaultPort);
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

	int ind = 0;
	char delim[] = "://";
	postgrurl* url = malloc(sizeof(postgrurl));

  //helper variables
	char *str_copy;
	char *value;
	char *query_split;
  char string_port[5];

	//query parts
	char *scheme;
  char *host;
	char *file;
  char *query;
  char *raw;
	int port = 0;

	//check variables
	int with_protocol;
	int with_port;
	int with_file;
	int with_query;


	//memory allocation
	scheme = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(scheme,"");
	host = malloc(sizeof(char) * (strlen(str)+1));
	file = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(file,"");
	query = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(query,"");
  raw = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(raw,"");
	str_copy = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(str_copy,str);

	query_split = malloc(sizeof(char) * (strlen(str)+1));

	//determine which parts are contained in the URL
	with_protocol = strstr(str,"://");
	if(with_protocol!= NULL)
	{
		char *ptr = strtok(str_copy, ":");
		char *url_part = str + strlen(ptr)+3;
		with_port = strstr(url_part,":");
		with_file = strstr(url_part,"/");
		with_query = strstr(url_part,"?");

	}else{
		with_port = strstr(str,":");
		with_file = strstr(str,"/");
		with_query = strstr(str,"?");
	}

	char *ptr = strtok(str, delim);

  //split the URL string into its individual components
	while(ptr != NULL)
	{
		value = ptr;
		switch(ind)
		{
			case 0:
				//first part is either host or protocol
				if(with_protocol!= NULL)
				{
					// https:// ...
					strcpy(scheme,value);
				}else
				{
					// host...
					strcpy(host,value);
				}
				break;

			case 1:
        //If first part was protocol second most be the host otherwise it
        //can be port,query or file
				if(with_protocol != NULL)
				{
					// https://host
					strcpy(host,value);
				}else
				{
					if(with_port!=NULL)
					{ //add check for valid port
						// host:port
						port = atoi(value);
            strcpy(string_port,value);

					}else if(strstr(value,"?") != NULL)
					{// host/file?query

						query_split = strtok(value, "?");
						strcpy(file+strlen(file),"/");
						strcpy(file+strlen(file), query_split);

						query_split = strtok(NULL, "?");
						strcpy(query+strlen(query),"?");
						strcpy(query+strlen(query), query_split);
					}else
					{	// host/file
						strcpy(file+strlen(file),"/");
						strcpy(file+strlen(file), value);
					}
				}
				break;

			case 2:
      //if URL contains protocol third part can either be the
      //port, query or file if URL doesnt contain protocol skip to default case
				if(with_protocol!= NULL)
				{
					if(with_port!=NULL)
					{	// protocol://host:port
						//Valid port check
						port = atoi(value);
            strcpy(string_port,value);
					}else if(strstr(value,"?") != NULL)
					{
						// protocol://host/file?query
						query_split = strtok(value, "?");
						strcpy(file+strlen(file),"/");
						strcpy(file+strlen(file), query_split);

						query_split = strtok(NULL, "?");
						strcpy(query+strlen(query),"?");
						strcpy(query+strlen(query), query_split);

					}else
					{	// protocol://host/file
						strcpy(file+strlen(file),"/");
						strcpy(file+strlen(file), value);
					}
					break;
				}

			default:
				//only possibilities left are file and query
        //check if remaining part contains a query if not everthing belongs to
        // the file part
        if(strstr(value,"?") != NULL)
				{	// portocol://host:port/file?query

					query_split = strtok(value, "?");
					strcpy(file+strlen(file),"/");
					strcpy(file+strlen(file), query_split);
					query_split = strtok(NULL, "?");
					strcpy(query+strlen(query),"?");
					strcpy(query+strlen(query), query_split);
				}else
        {
					// portocol://host:port/file
					strcpy(file+strlen(file),"/");
					strcpy(file+strlen(file), value);
				}
				break;
		}
		ptr = strtok(NULL, delim);
		ind++;
	}

  //Assign the components that were present in the URL string to URL struct
  // and create the raw string
  if(with_protocol!=0)
	{
		url->scheme = malloc(strlen(scheme) + 1);
		strcpy(url->scheme, scheme);
    strcpy(raw, scheme);
    strcpy(raw+strlen(raw), "://");
	}else
  {
    url->scheme = NULL;
  }

	if(strcmp(host,"")!=0)
	{
		url->host = malloc(strlen(host) + 1);
		strcpy(url->host,host);
		strcpy(raw+strlen(raw), host);
	}else
  {
    url->host = NULL;
  }

  if(with_port!=0)
  {
    url->port = port;
    strcpy(raw+strlen(raw), ":");
    strcpy(raw+strlen(raw),string_port);
  } else if (with_protocol!=0)
  {
    //protocol but no port .-> assign default port
    url->defaultPort = assignDefaultPort(scheme);
  }

	if(with_file!=0)
	{
		url->file = malloc(strlen(file) + 1);
		strcpy(url->file,file);
    strcpy(raw+strlen(raw), file);
	}else
  {
    url->file = NULL;
  }

	if(with_query!=0)
	{
		url->query = malloc(strlen(query) + 1);
		strcpy(url->query,query);
    strcpy(raw+strlen(raw), query);
	}else
  {
    url->query = NULL;
  }

  //Assign raw string to struct
  url->raw = malloc(strlen(raw) + 1);
  strcpy(url->raw, raw);

	//free memory
	free(scheme);
	free(host);
	free(file);
	free(query);
  free(raw);
	free(str_copy);

  //Problem
	//free(query_split);

	return url;

}


//Functions
Datum url_in(PG_FUNCTION_ARGS);
Datum url_out(PG_FUNCTION_ARGS);
Datum URL_constructor_str(PG_FUNCTION_ARGS);
Datum URL_constructor1(PG_FUNCTION_ARGS);
Datum URL_constructor2(PG_FUNCTION_ARGS);
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
Datum sameFile(PG_FUNCTION_ARGS);
Datum sameHost(PG_FUNCTION_ARGS);
Datum toString(PG_FUNCTION_ARGS);

/*
Postgres type functions definition
*/

PG_FUNCTION_INFO_V1(url_in);
Datum url_in(PG_FUNCTION_ARGS){
    char *rawstr = PG_GETARG_CSTRING(0);
    postgrurl *url;
    url = (postgrurl *) palloc(sizeof(postgrurl));
    url = string_to_url(rawstr);
    PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(url_out);
Datum url_out(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = url_to_string(url);
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
}

PG_FUNCTION_INFO_V1(URL_constructor_str);
Datum URL_constructor_str(PG_FUNCTION_ARGS){
  postgrurl *url;
  char *rawstr = PG_GETARG_CSTRING(0);
  url = (postgrurl *) palloc(sizeof(postgrurl));
  url = string_to_url(rawstr);

  PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(URL_constructor1);
Datum URL_constructor1(PG_FUNCTION_ARGS){
  postgrurl *url;
  char *protocol = PG_GETARG_CSTRING(0);
  char *host = PG_GETARG_CSTRING(1);
  int port = PG_GETARG_INT32(2);
  char *file = PG_GETARG_CSTRING(3);

  url = (postgrurl *) palloc(sizeof(postgrurl));
  url = new_URL1(protocol,host,port,file);
  PG_RETURN_POINTER(url);
}


PG_FUNCTION_INFO_V1(URL_constructor2);
Datum URL_constructor2(PG_FUNCTION_ARGS){
  postgrurl *url;
  char *protocol = PG_GETARG_CSTRING(0);
  char *host = PG_GETARG_CSTRING(1);
  char *file = PG_GETARG_CSTRING(2);

  url = (postgrurl *) palloc(sizeof(postgrurl));
  url = new_URL2(protocol,host,file);
  PG_RETURN_POINTER(url);
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
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    if(url1->file != NULL && url2->file != NULL){
        int eq = strcmp(url1->file, url2->file);
        if(eq==0){
            PG_RETURN_BOOL(true);
        }
        PG_RETURN_BOOL(false);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(sameHost);
Datum sameHost(PG_FUNCTION_ARGS){
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    if(url1->host != NULL && url2->host != NULL){
        int eq = strcmp(url1->file, url2->file);
        if(eq==0){
            PG_RETURN_BOOL(true);
        }
        PG_RETURN_BOOL(false);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(toString);
Datum toString(PG_FUNCTION_ARGS){
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = url_to_string(url);
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
}
