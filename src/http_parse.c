#include "http_parse.h"
#include <sys/types.h>

enum http_method {
	HTTP_GET,
	HTTP_POST,
	HTTP_HEAD,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_UNKNOWN
};

enum http_version{
	HTTP_1_0,
	HTTP_1_1,
	HTTP_2_0,
	HTTP_UNSUPPORTED
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
	enum http_method method;		//HTTP_GET
	enum http_version version;		//HTTP
	char* host;						// www.example.com 
	char* request_target;			//"/index.html?id=43"
	char* path;						//"/index.html"
	enum connection connection;		//KEEP_ALIVE
	enum transfer_encoding encode;	//CHUNKED
	ssize_t content_length;			//123	(-1 for no content_length)
};
