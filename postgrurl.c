/************************************************************************************************************
*    C Code that implements a URL handler for Postgres extensions. This handler mimics java.net.URL class.   *
*    Authors:                                                                                                *
*        Luis Alfredo Leon - luis.leon.villapun@ulb.be                                                       *
*        Maren Hoschek - maren.hoschek@ulb.be                                                                *
*        Satria Bagus - satria.wicaksono@ulb.be                                                              *
*        Sayyor Yusupov - sayyor.yusupov@ulb.be                                                              *
*************************************************************************************************************/

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

/*********************************************************************************
*  Type declaration                                                              *
**********************************************************************************/

struct postgrurl{
    char *raw;
    char *scheme;
    char *host;
    char *file;
    char *query;
    int port;
    int defaultPort;
};
typedef struct postgrurl postgrurl;

/*********************************************************************************
* Helper functions                                                               *
**********************************************************************************/

int digits_only(const char *s){
    /*
        Check if a string only contains digits (for recognizing the port).
    */
    while (*s){
        if (isdigit(*s++) == 0) return 0;
    }
    return 1;
}

int assignDefaultPort(const char *protocol){
    /*
        Gets the default port for a protocol given common protocol schemes.
    */
	int defaultPort;
	if (strcmp(protocol, "http") == 0){
		defaultPort = 80;
	}
    else if(strcmp(protocol, "https") == 0){
		defaultPort = 443;

  }else if(strcmp(protocol, "ftp") == 0){
    defaultPort = 21;

  }else if(strcmp(protocol, "sftp") == 0){
    defaultPort = 22;
  }
  else if(strcmp(protocol, "gopher") == 0){
    defaultPort = 70;
  }

  else if(strcmp(protocol, "imap") == 0){
    defaultPort = 143;
  }

  else if(strcmp(protocol, "ldap") == 0){
    defaultPort = 389;
  }

  else if(strcmp(protocol, "nfs") == 0){
    defaultPort = 2049;
  }

  else if(strcmp(protocol, "nntp") == 0){
    defaultPort = 119;
  }

  else if(strcmp(protocol, "pop") == 0){
    defaultPort = 110;
  }

  else if(strcmp(protocol, "smtp") == 0){
    defaultPort = 25;
  }

  else if(strcmp(protocol, "telnet") == 0){
    defaultPort = 23;
  }

  else{
		defaultPort = 0;
	}
	return defaultPort;
}

postgrurl* string_to_url(char* str){
    /*
        Converts a given string into a url representation.
    */
	int ind = 0;
	char delim[] = "://";
	postgrurl* url = palloc(sizeof(postgrurl));

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
  int end_slash=0;


	//memory allocation
	scheme = malloc(sizeof(char) * (strlen(str)+1));
	strcpy(scheme,"");
	host = malloc(sizeof(char) * (strlen(str)+1));
    strcpy(host,"");
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
	if(with_protocol!= NULL){
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
    //check if file part ends with /
	if(with_file != NULL){
		strcpy(file+strlen(file),"/");
		if(strstr(str,"/?")!= NULL || str[strlen(str)-1]=='/'){
			end_slash = 1;
		}
	}

	char *ptr = strtok(str, delim);

    //split the URL string into its individual components
	while(ptr != NULL){
		value = ptr;
		switch(ind){
			case 0:
				//first part is either host or protocol
				if(with_protocol!= NULL){
					// https:// ...
					strcpy(scheme,value);
				}else{
					// host...
                    if(strstr(value,"?") != NULL){
                        //host?query
                        query_split = strtok(value, "?");
                        strcpy(host+strlen(host), query_split);

                        query_split = strtok(NULL, "?");
                        strcpy(query+strlen(query),"?");
                        strcpy(query+strlen(query), query_split);
                    }
                    else{
                        // host...
                        strcpy(host+strlen(host),value);
                    }
				}
				break;
			case 1:
                //If first part was protocol second most be the host otherwise it
                //can be port,query or file
				if(with_protocol != NULL){
                    if(strstr(value,"?") != NULL){
						// https://host?query
						query_split = strtok(value, "?");
						strcpy(host+strlen(host), query_split);

						query_split = strtok(NULL, "?");
						strcpy(query+strlen(query),"?");
						strcpy(query+strlen(query), query_split);
					}else{
						// https://host
						strcpy(host+strlen(host),value);
                    }
				}
                else{
					if(with_port!=NULL){ //add check for valid port
                        if(strstr(value,"?") != NULL){
							// host:port?query
							query_split = strtok(value, "?");
							port = atoi(query_split);
                            strcpy(string_port,query_split);

							query_split = strtok(NULL, "?");
							strcpy(query+strlen(query),"?");
							strcpy(query+strlen(query), query_split);

						}
                        else{
							// host:port
							port = atoi(value);
                            strcpy(string_port,value);
						}
					}
                    else if(strstr(value,"?") != NULL){
                        // host/file?query
                        if(value[0]=='?'){
							// host/file/?query
							strcpy(query+strlen(query),value);
						}
                        else {
							// host/file?query
							query_split = strtok(value, "?");
							strcpy(file+strlen(file), query_split);
							strcpy(file+strlen(file),"/");

							query_split = strtok(NULL, "?");
							strcpy(query+strlen(query),"?");
							strcpy(query+strlen(query), query_split);
						}
					}
                    else{
                        // host/file
                        strcpy(file+strlen(file), value);
						strcpy(file+strlen(file),"/");
					}
				}
				break;

			case 2:
                //if URL contains protocol third part can either be the
                //port, query or file if URL doesnt contain protocol skip to default case
				if(with_protocol!= NULL){
					if(with_port!=NULL){	// protocol://host:port
						//Valid port check
                        if(strstr(value,"?") != NULL){
							// protocol://host:port
							query_split = strtok(value, "?");
							port = atoi(query_split);
                            strcpy(string_port,query_split);
							query_split = strtok(NULL, "?");
							strcpy(query+strlen(query),"?");
							strcpy(query+strlen(query), query_split);
						}
                        else{
							// host:port?query
                            port = atoi(value);
                            strcpy(string_port,value);
						}
					}
                    else if(strstr(value,"?") != NULL){
                        if(value[0]=='?'){
							// protocol://host/file/?query
							strcpy(query+strlen(query),value);
						}
                        else{
							// protocol://host/file?query
							query_split = strtok(value, "?");
							strcpy(file+strlen(file), query_split);
							strcpy(file+strlen(file),"/");

							query_split = strtok(NULL, "?");
							strcpy(query+strlen(query),"?");
							strcpy(query+strlen(query), query_split);
						}

					}
                    else{
                        // protocol://host/file
                        strcpy(file+strlen(file), value);
						strcpy(file+strlen(file),"/");
					}
					break;
				}
			default:
				//only possibilities left are file and query
                //check if remaining part contains a query if not everthing belongs to
                // the file part
                if(strstr(value,"?") != NULL){
                    // portocol://host:port/file?query
                    if(value[0]=='?'){
						// portocol://host:port/file/?query
						strcpy(query+strlen(query),value);
					}
                    else{
						// portocol://host:port/file?query
						printf("Value: %s \n", value);
						query_split = strtok(value, "?");

						strcpy(file+strlen(file), query_split);
						strcpy(file+strlen(file),"/");

						query_split = strtok(NULL, "?");
						strcpy(query+strlen(query),"?");
						strcpy(query+strlen(query), query_split);
					}
				}
                else{
					// portocol://host:port/file
					strcpy(file+strlen(file), value);
					strcpy(file+strlen(file),"/");
				}
				break;
		}
		ptr = strtok(NULL, delim);
		ind++;
	}

    //if neccesary remove last slash
    if(end_slash == 0){
        file[strlen(file)-1] = '\0';
    }

    //Assign the components that were present in the URL string to URL struct
    // and create the raw string
    if(with_protocol!=0){
		url->scheme = malloc(strlen(scheme) + 1);
		strcpy(url->scheme, scheme);
        strcpy(raw, scheme);
        strcpy(raw+strlen(raw), "://");
	}
    else{
        url->scheme = NULL;
    }

	if(strcmp(host,"")!=0){
		url->host = malloc(strlen(host) + 1);
		strcpy(url->host,host);
		strcpy(raw+strlen(raw), host);
	}
    else{
        url->host = NULL;
    }

    if(with_port!=0){
        url->port = port;
        strcpy(raw+strlen(raw), ":");
        strcpy(raw+strlen(raw),string_port);
    }
    else if(with_protocol!=0){
        //protocol but no port .-> assign default port
        url->defaultPort = assignDefaultPort(scheme);
    }

	if(with_file!=0){
		url->file = malloc(strlen(file) + 1);
		strcpy(url->file,file);
        strcpy(raw+strlen(raw), file);
	}
    else{
        url->file = NULL;
    }

	if(with_query!=0){
		url->query = malloc(strlen(query) + 1);
		strcpy(url->query,query);
        strcpy(raw+strlen(raw), query);
	}
    else{
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

char * url_to_string(postgrurl* url){
    /*
        Converts a url struct into a readable string representation.
    */
    char * port = (char *) palloc(6*sizeof(char));
    char *defaultPort = (char *) palloc(6*sizeof(char));
    char * stringed_url = (char *) palloc(1024*sizeof(char));
    strcpy(stringed_url, "");
    if(url->raw != NULL){
        strcat(stringed_url, url->raw);
    }
    pfree(port);
    pfree(defaultPort);
    return stringed_url;
}

int _sameHost(postgrurl* url1, postgrurl* url2){
    /*
        Checks if hosts between two urls are the same.
        If left url is lower than right url, returns -1.
        If left url is greater than right url, returns 1.
        If left url equals right url, returns 0.
    */
    if(url1->host != NULL && url2->host != NULL){
        int eq = strcmp(url1->host, url2->host);
        return eq;
    }
    else if(url1->host == NULL && url2->host != NULL){
        return -1;
    }
    else if(url1->host != NULL && url2->host == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int _sameFile(postgrurl* url1, postgrurl* url2){
    /*
        Checks if files between two urls are the same.
        If left url is lower than right url, returns -1.
        If left url is greater than right url, returns 1.
        If left url equals right url, returns 0.
    */
    if(url1->file != NULL && url2->file != NULL){
        int eq = strcmp(url1->file, url2->file);
        return eq;
    }
    else if(url1->file == NULL && url2->file != NULL){
        return -1;
    }
    else if(url1->file != NULL && url2->file == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int _equals(postgrurl* url1, postgrurl* url2){
    /*
        Checks if two urls are the same.
        If left url is lower than right url, returns -1.
        If left url is greater than right url, returns 1.
        If left url equals right url, returns 0.
    */
    if(url1->raw != NULL && url2->raw != NULL){
        int eq = strcmp(url1->raw, url2->raw);
        return eq;
    }
    else if(url1->raw == NULL && url2->raw != NULL){
        return -1;
    }
    else if(url1->raw != NULL && url2->raw == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int _cmp(postgrurl* url1, postgrurl* url2){
    /*
        Compares a pair of urls.
    */
    int sameHost = _sameHost(url1, url2);
    if(sameHost == 0){
        int sameFile = _sameFile(url1, url2);
        if(sameFile == 0){
            int sameRaw = _equals(url1, url2);
            if(sameRaw==0){
                return 0;
            }
            else if(sameRaw<0){
                return -1;
            }
            else{
                return 1;
            }
        }
        else if(sameFile < 0){
            return -2;
        }
        else if(sameFile > 0){
            return 2;
        }
    }
    else if(sameHost < 0){
        return -3;
    }
    else if(sameHost > 0){
        return 3;
    }
}


/*********************************************************************************
* Constructors                                                                   *
**********************************************************************************/
postgrurl* URLFromString(char* rawString){
    /*
        Constructs a url struct based on a string representation.
    */
    postgrurl *url = (postgrurl *) palloc(sizeof(postgrurl));
    url = string_to_url(rawString);
    return url;
}

postgrurl* URLFromProtocolHostPortFile(char* protocol, char* host, int port, char* file) {
    /*
        Constructor with protocol, host, port, file.
    */
    postgrurl* url = (postgrurl*) palloc(sizeof(postgrurl));
    int raw_size = sizeof(char) * ((strlen(protocol)+strlen(host)+strlen(file)+5+5+1));
    char *raw = (char *) palloc(raw_size);

    url->scheme = strdup(protocol);
    url->host = strdup(host);
	url->file = strdup(file);
	url->port = port;
    int defaultPort = assignDefaultPort(protocol);
    url->defaultPort=defaultPort;

    snprintf(raw, raw_size, "%s://%s:%d/%s", url->scheme, url->host, url->port, url->file);
    url->raw = strdup(raw);
    pfree(raw);
    return url;
}

postgrurl* URLFromProtocolHostFile(char* protocol, char* host, char* file) {
    /*
        Constructor with protocol, host and file.
    */
    postgrurl* url = (postgrurl*) palloc(sizeof(postgrurl));
    int raw_size = sizeof(char) * ((strlen(protocol)+strlen(host)+strlen(file)+ 5+5+1));
    char *raw = (char *) palloc(raw_size);
    url->scheme = strdup(protocol);
    url->host = strdup(host);
    url->file = strdup(file);
    int defaultPort = assignDefaultPort(protocol);
    url->defaultPort = defaultPort;
    url->port = 0; // TODO: Change to null if it doesn't break other code.

    snprintf(raw, raw_size, "%s://%s/%s", protocol, host, file);
    url->raw = strdup(raw);
    pfree(raw);
    return url;
}

postgrurl* URLFromContextAndSpec(postgrurl* context, char* spec) {
    /*
        Constructor based on a context and spec.
    */

    //Check whether spec contains scheme or not
    int index;
    char delimiter[3] = "://";
    char *pos = strstr(spec, delimiter);
    index = pos ? pos - spec : -1;

    if (index != -1 || index > 0) {
        // Convert spec to URL
        postgrurl* spec_url = (postgrurl*) palloc(sizeof(postgrurl));
        spec_url = string_to_url(spec);

        // Spec is Absolute URL
        if(strcmp(spec_url->scheme, context->scheme) != 0 || spec_url->host != NULL) {
            return spec_url;
        }

        pfree(spec_url);

        // Transform new raw value & copy context to url
        char * raw = palloc(1024 * sizeof(char));
        postgrurl* url = (postgrurl*) palloc(sizeof(postgrurl));

        strcpy(raw, "");

        if (context->scheme) {
            url->scheme = strdup(context->scheme);
            strcat(raw, url->scheme);
            strcat(raw, "://");
        }

        if (context->host) {
            url->host = strdup(context->host);
            strcat(raw, url->host);
        }

        if(context->port) {
            url->port = context->port;
            strcat(raw, ":");
            strcat(raw, url->port);
        }

        if(context->file) {
            url->file = strdup(context->file);
            strcat(raw, url->file);
        }

        if(context->query) {
            url->query = strdup(context->query);
            strcat(raw, url->query);
        }
        url->raw = strdup(raw);
        pfree(raw);
            
        return url;
    }

    // Case spec is path
    // Get file and query from spec
    char * query_split;
    char * query = (char *) palloc((strlen(spec) + 1 ) *sizeof(char));
    char * file = (char *)  palloc(1024* sizeof(char));

    if(strstr(spec,"?") != NULL) {
        query_split = strtok(spec, "?");
        strcpy(file, query_split);

        query_split = strtok(NULL, "?");
        strcpy(query, "?");
        strcpy(query+strlen(query), query_split);
    } else {
        strcpy(file, spec);

        // Empty query
        strcpy(query,"");
    }

    // Append if spec is not absoulte path otherwise overwrite context path
    if (spec[0] != '/') {
        char * new_file = palloc((strlen(context->file)+1)*sizeof(char));
        new_file = strcpy(new_file, context->file);

        // Determine which part of the file need to be replaced
        char * last;
        int index;
        int ignore_last;

        last = strrchr(new_file, '/');
        index = last ? last - new_file : -1;
        ignore_last = index == strlen(new_file)-1 ? 0 : 1;

        char delim[] = "/";
        char ** file_part  = palloc(sizeof(char));
        char *  ptr    = strtok (new_file, delim);
        int n_spaces = 0;

        // Split string token into array, taken from:
        // https://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings
        while (ptr) {
            file_part = repalloc (file_part, sizeof (char*) * ++n_spaces);

            if (file_part == NULL)
                exit (-1);

            file_part[n_spaces-1] = ptr;
            ptr = strtok (NULL, delim);

            if(!ptr && ignore_last) {
                file_part[n_spaces-1] = 0;
            }
        }

        // Add null
        file_part = repalloc (file_part, sizeof (char*) * (n_spaces+1));
        file_part[n_spaces] = 0;

        // Combine file and new path
        char * combined_file;
        combined_file = palloc(1024*sizeof(char));
        strcpy(combined_file, "");

        for (int i = 0; i < (n_spaces+1); ++i) {
            if (file_part[i]) {
                strcat(combined_file, "/");
                strcat(combined_file, file_part[i]);
            }
        }

        // Combine path with file
        strcat(combined_file, "/");
        strcat(combined_file, file);
        strcpy(file, combined_file);

        pfree(file_part);
        pfree(new_file);
        pfree(combined_file);
    }

    // Transform new raw value
    char * raw = (char *)  palloc(1024 * sizeof(char));
    postgrurl* url = (postgrurl*) palloc(sizeof(postgrurl));

    strcpy(raw, "");

    if (context->scheme) {
        url->scheme = strdup(context->scheme);
        strcat(raw, url->scheme);
        strcat(raw, "://");
    }

    if (context->host) {
        url->host = strdup(context->host);
        strcat(raw, url->host);
    }

    if(context->port) {
        url->port = context->port;
        strcat(raw, ":");
        strcat(raw, url->port);
    }

    if(file) {
        url->file = strdup(file);
        strcat(raw, file);
    }

    if(query) {
        url->query = strdup(query);
        strcat(raw, query);
    }
    url->raw = strdup(raw);

    pfree(query);
    pfree(raw);
    pfree(file);

    return url;
}


/*********************************************************************************
* Postgres constructors                                                          *
**********************************************************************************/

Datum URLPostgresFromString(PG_FUNCTION_ARGS);
Datum URLPostgresFromProtocolHostPortFile(PG_FUNCTION_ARGS);
Datum URLPostgresFromProtocolHostFile(PG_FUNCTION_ARGS);
Datum URLPostgresFromContext(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(URLPostgresFromString);
Datum URLPostgresFromString(PG_FUNCTION_ARGS){
    /*
        Constructs an URL based on a raw string.
    */
    char *rawstr = PG_GETARG_CSTRING(0);
    postgrurl *url;
    url = URLFromString(rawstr);
    pfree(rawstr);
    PG_RETURN_POINTER(url);
    //pfree(url);
}

PG_FUNCTION_INFO_V1(URLPostgresFromProtocolHostPortFile);
Datum URLPostgresFromProtocolHostPortFile(PG_FUNCTION_ARGS){
    /*
        Constructs an URL based on a protocol, host, port, and file.
    */
    postgrurl * url;
    char *protocol = PG_GETARG_CSTRING(0);
    char *host = PG_GETARG_CSTRING(1);
    int port = PG_GETARG_INT32(2);
    char *file = PG_GETARG_CSTRING(3);
    url = URLFromProtocolHostPortFile(protocol,host,port,file);
    pfree(protocol);
    pfree(host);
    pfree(file);
    PG_RETURN_POINTER(url);
    //pfree(url);
}


PG_FUNCTION_INFO_V1(URLPostgresFromProtocolHostFile);
Datum URLPostgresFromProtocolHostFile(PG_FUNCTION_ARGS){
    /*
        Constructs an URL based on protocol, host, and file.
    */
    postgrurl *url;
    char *protocol = PG_GETARG_CSTRING(0);
    char *host = PG_GETARG_CSTRING(1);
    char *file = PG_GETARG_CSTRING(2);
    url = URLFromProtocolHostFile(protocol,host,file);
    pfree(protocol);
    pfree(host);
    pfree(file);
    PG_RETURN_POINTER(url);
    //pfree(url);
}

PG_FUNCTION_INFO_V1(URLPostgresFromContext);
Datum URLPostgresFromContext(PG_FUNCTION_ARGS) {
    /*
        Constructor that receives an existing url and a context.
    */
    postgrurl *context = (postgrurl *) PG_GETARG_POINTER(0);
    char *spec = PG_GETARG_CSTRING(1);

    postgrurl *url = URLFromContextAndSpec(context, spec);
    pfree(spec);
    pfree(context);

    PG_RETURN_POINTER(url);
}

/*********************************************************************************
* Postgres type functions                                                        *
**********************************************************************************/


Datum url_in(PG_FUNCTION_ARGS);
Datum url_out(PG_FUNCTION_ARGS);
Datum url_recv(PG_FUNCTION_ARGS);
Datum url_send(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(url_in);
Datum url_in(PG_FUNCTION_ARGS){
    /*
        Native input function for custom data type.
        The default way to create a postgrurl data cell will be by passing the raw string.
    */
    char *rawstr = PG_GETARG_CSTRING(0);
    postgrurl *url;
    url = URLFromString(rawstr);
    pfree(rawstr);
    PG_RETURN_POINTER(url);
    pfree(url);
}

PG_FUNCTION_INFO_V1(url_out);
Datum url_out(PG_FUNCTION_ARGS){
    /*
        Native output function for custom data type.
        Will print the raw string representation of the saved postgrurl data cell.
    */
    postgrurl *url = PG_GETARG_POINTER(0);
    char *output = url_to_string(url);
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}

PG_FUNCTION_INFO_V1(url_recv);
Datum url_recv(PG_FUNCTION_ARGS){
    /*
        Native rcv function for custom data type.
    */
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    const char *str = pq_getmsgstring(buf);
    pq_getmsgend(buf);
    
    postgrurl *url;
    url = URLFromString(str);

    PG_RETURN_POINTER(url);
}

PG_FUNCTION_INFO_V1(url_send);
Datum url_send(PG_FUNCTION_ARGS){
    /*
        Native send function for custom data type.
    */
    postgrurl *url = PG_GETARG_POINTER(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendstring(&buf, url_to_string(url));

    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*********************************************************************************
* Postgres comparison functions                                                  *
**********************************************************************************/

Datum equals(PG_FUNCTION_ARGS);
Datum greater_than(PG_FUNCTION_ARGS);
Datum less_than(PG_FUNCTION_ARGS);
Datum greater_than_equals(PG_FUNCTION_ARGS);
Datum less_than_equals(PG_FUNCTION_ARGS);
Datum cmp(PG_FUNCTION_ARGS);
Datum not_equals(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(equals);
Datum equals(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if they are equal or not.
        Only case when two URLs are equal is when the full raw string representation is the same.
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _cmp(url1, url2);
    if(eq==0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(greater_than);
Datum greater_than(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if url1 is greater than url2.
        A url A is considered greater than url B when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns positive number.
                Example: http://test.com/about/file.txt?q=user2 vs http://test.com/about/file.txt?q=user
            - A and B share host, and file string comparison returns positive number.
                Example: http://test.com/about/file2.txt vs http://test.com/about/file.txt
            - A and B host comparison returns positive number.
                Example: http://test.com/ vs http://exam.com/
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _cmp(url1, url2);
    if(eq>0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(less_than);
Datum less_than(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if url1 is less than url2.
        A url A is considered less than url B when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns negative number.
                Example: http://test.com/about/file.txt?q=user vs http://test.com/about/file.txt?q=user2
            - A and B share host, and file string comparison returns negative number.
                Example:  http://test.com/about/file.txt http://test.com/about/file2.txt
            - A and B host comparison returns negative number.
                Example:  http://exam.com/ vs http://test.com/
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _cmp(url1, url2);
    if(eq<0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(greater_than_equals);
Datum greater_than_equals(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if url1 is greater than or equal to url2.
        A url A is considered greater than url B when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns positive number or 0.
                Example: http://test.com/about/file.txt?q=user2 vs http://test.com/about/file.txt?q=user
            - A and B share host, and file string comparison returns positive number or 0.
                Example: http://test.com/about/file2.txt vs http://test.com/about/file.txt
            - A and B host comparison returns positive number or 0.
                Example: http://test.com/ vs http://exam.com/
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _cmp(url1, url2);
    if(eq>=0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(less_than_equals);
Datum less_than_equals(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if url1 is less than url2.
        A url A is considered less or equal than url B when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns negative number or 0.
                Example: http://test.com/about/file.txt?q=user vs http://test.com/about/file.txt?q=user2
            - A and B share host, and file string comparison returns negative number or 0.
                Example:  http://test.com/about/file.txt http://test.com/about/file2.txt
            - A and B host comparison returns negative number or 0.
                Example:  http://exam.com/ vs http://test.com/
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _cmp(url1, url2);
    if(eq<=0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(cmp);
Datum cmp(PG_FUNCTION_ARGS){
    /*
        Calls directly the compare function for urls.
        Returns 0 when the raw representation of both urls is the same (same host, same file, and same rest of url).
        Returns negative number when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns negative number.
            - A and B share host, and file string comparison returns negative number.
            - A and B have different host, and host string comparison returns negative number.
        Returns a positive number when:
            - A and B share host, share file, but the rest of the url is different and string comparison returns positive number.
            - A and B share host, and file string comparison returns positive number.
            - A and B have different host, and host string comparison returns positive number.
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int cmp = _cmp(url1, url2);
    PG_RETURN_INT32(cmp);
    pfree(url1);
    pfree(url2);
}

PG_FUNCTION_INFO_V1(not_equals);
Datum not_equals(PG_FUNCTION_ARGS){
    /*
        Compares two urls and determines if they are not equal.
        Only case when two URLs are equal is when the full raw string representation is the same.
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _equals(url1, url2);
    //pfree(url1);
    //pfree(url2);
    if(eq==0){
        PG_RETURN_BOOL(false);
    }
    PG_RETURN_BOOL(true);
}


/*********************************************************************************
* Postgres helper functions                                                      *
**********************************************************************************/

Datum getAuthority(PG_FUNCTION_ARGS);
Datum getFile(PG_FUNCTION_ARGS);
Datum getHost(PG_FUNCTION_ARGS);
Datum getPort(PG_FUNCTION_ARGS);
Datum getPath(PG_FUNCTION_ARGS);
Datum getProtocol(PG_FUNCTION_ARGS);
Datum getQuery(PG_FUNCTION_ARGS);
Datum getRef(PG_FUNCTION_ARGS);
Datum sameFile(PG_FUNCTION_ARGS);
Datum sameHost(PG_FUNCTION_ARGS);
Datum toString(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(getAuthority);
Datum getAuthority(PG_FUNCTION_ARGS){
    /*
        Gets the authority part of a url.
    */
    PG_RETURN_NULL();
}


PG_FUNCTION_INFO_V1(getDefaultPort);
Datum getDefaultPort(PG_FUNCTION_ARGS){
    /*
        Returns the default port of the url scheme, if detected.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    if(url->defaultPort != NULL){
        PG_RETURN_INT32(url->defaultPort);
    }
    else{
        if(url->scheme != NULL){
            int defaultPort = assignDefaultPort(url->scheme);
            if(defaultPort == 0){
                ereport(ERROR,(errmsg("No default port found for provided scheme.")));
            }
            else{
                url->defaultPort = defaultPort;
                PG_RETURN_INT32(defaultPort);
            }
        }
        ereport(ERROR,(errmsg("No scheme provided in url. No default port.")));
    }
    pfree(url);
}

PG_FUNCTION_INFO_V1(getFile);
Datum getFile(PG_FUNCTION_ARGS){
    /*
        Returns the file part of the url.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output;
    if(url->file != NULL){
        output = (char *) palloc((strlen(url->file)+1)*sizeof(char));
        strcpy(output, url->file);
    }
    else{
        ereport(ERROR,
            (
             errmsg("No file in the url.")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getHost);
Datum getHost(PG_FUNCTION_ARGS){
    /*
        Returns the host part of the url.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output;
    if(url->host != NULL){
        output = (char *) palloc((strlen(url->host)+1)*sizeof(char));
        strcpy(output, url->host);
    }
    else{
        ereport(ERROR,
            (
             errmsg("No host in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getPath);
Datum getPath(PG_FUNCTION_ARGS){
    /*
        Returns the path part of the url.
    */
    PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(getPort);
Datum getPort(PG_FUNCTION_ARGS){
    /*
        Returns the port of the url, if exists.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    int output;

    if(url->port != NULL){
        output = url->port;
        PG_RETURN_INT32(output);
    }
    else{
        ereport(ERROR,
            (
             errmsg("No port in the url.")
            )
        );
    }
    pfree(output);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getProtocol);
Datum getProtocol(PG_FUNCTION_ARGS){
    /*
        Returns the protocol of the url, if exists.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output;
    if(url->scheme != NULL){
        output = (char *) palloc((strlen(url->scheme) +1)*sizeof(char));
        strcpy(output, url->scheme);
    }
    else{
        ereport(ERROR,
            (
             errmsg("No protocol in the url")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getQuery);
Datum getQuery(PG_FUNCTION_ARGS){
    /*
        Returns the query of the url, if exists.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output;
    if(url->query != NULL){
        output = (char *) palloc((strlen(url->query)+1)*sizeof(char));
        strcpy(output, url->query);
    }
    else{
        ereport(ERROR,
            (
             errmsg("No query in the url.")
            )
        );
    }
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getRef);
Datum getRef(PG_FUNCTION_ARGS){
    /*
        Returns the ref of the url, if exists.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char * output;
    char delim[] = "#";
    output = (char *) palloc((strlen(url->raw)+1)*sizeof(char));
    output = strtok(url->raw, delim); //
    output = strtok(NULL, delim);
    if (output == NULL) {
        ereport(ERROR,
            (
             errmsg("No reference in the url")
            )
        );
    }
    else{
        printf("%s", output);
    }
    PG_RETURN_CSTRING(output);
    pfree(output);
    pfree(delim);
    pfree(url);
}

PG_FUNCTION_INFO_V1(getUserInfo);
Datum getUserInfo(PG_FUNCTION_ARGS){
    /*
        Returns the user info through the url, if exists.
    */
    PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(sameFile);
Datum sameFile(PG_FUNCTION_ARGS){
    /*
        Returns true if file part of urls are equal.
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _sameFile(url1, url2);
    if(eq == 0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(sameHost);
Datum sameHost(PG_FUNCTION_ARGS){
    /*
        Returns true if host part of urls are equal.
    */
    postgrurl *url1 = (postgrurl *) PG_GETARG_POINTER(0);
    postgrurl *url2 = (postgrurl *) PG_GETARG_POINTER(1);
    int eq = _sameHost(url1, url2);
    if(eq == 0){
        PG_RETURN_BOOL(true);
    }
    PG_RETURN_BOOL(false);
}

PG_FUNCTION_INFO_V1(toString);
Datum toString(PG_FUNCTION_ARGS){
    /*
        Parses url to raw string format.
    */
    postgrurl *url = (postgrurl *) PG_GETARG_POINTER(0);
    char *output = url_to_string(url);
    output = psprintf("%s", output);
    PG_RETURN_CSTRING(output);
    pfree(output);
}
