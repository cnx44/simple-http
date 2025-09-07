#ifndef SERVER_H 
#define SERVER_H


#define PORT 8080
#define MESSAGE_SIZE 8192
#define LINE_SIZE 512

struct environment_var{
	char* pages_folder;
	char* index;
	char* fav_ico;
	char* error_pages;
};

extern struct environment_var env;

#endif
