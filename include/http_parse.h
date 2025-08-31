#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define MSG_LENGTH 819
#define WRD_LENGTH MSG_LENGTH / 8

enum http_method {
	HTTP_GET,
	HTTP_POST,
	HTTP_HEAD,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_UNKNOWN
};

enum http_version{
	HTTP_1_0 = 10,
	HTTP_1_1 = 11,
	HTTP_2_0 = 20,
	HTTP_UNSUPPORTED = -1
};

enum connection{
	KEEP_ALIVE,
	CLOSE
};

enum transfer_encoding{
	CHUNKED, 
	NOT_IMPLEMENTED
};

struct request_header{				//EXAMPLE:
	enum http_method method;		//HTTP_GET done
	enum http_version version;		//HTTP done
	char* host;						// www.example.com done
	char* request_target;			//"/index.html?id=43" done
	char* path;						//"/index.html"
	char* query;					// id=43
	enum connection connection;		//KEEP_ALIVE done
	enum transfer_encoding encode;	//CHUNKED
	ssize_t content_length;			//123	(-1 for no content_length)
};

void header_parser(char* request_message, struct request_header* request_ptr);


#endif
