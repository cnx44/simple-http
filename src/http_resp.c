#include "http_resp.h"


/*
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



*/

char* header_builder(struct request_header* request_hd){
	if(!request_hd) return NULL;
	

	char* BASIC_HEADER = malloc(512 * sizeof(char));
	BASIC_HEADER =	"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Connection: close\r\n"
					"\r\n";

	return  BASIC_HEADER;
}

void message_sender(int client_fd, char* message, struct request_header* request_hd){
	char ret_msg[512];
	
	char* BASIC_HEADER = header_builder(request_hd);
	char* OK_MESSAGE_OPEN = "<html><body><h1>200 OK! ";
	char* OK_MESSAGE_CLOSE= "</h1></body></html>";
	
	strcat(ret_msg, BASIC_HEADER);
	strcat(ret_msg, OK_MESSAGE_OPEN);
	strcat(ret_msg, message);
	strcat(ret_msg, OK_MESSAGE_CLOSE);

	int written_byte = write_socket(client_fd, ret_msg, strlen(ret_msg));
	if(written_byte == -1) fprintf(stderr, "Error on writing socket");
}

