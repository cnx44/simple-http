#include "http_parse.h"

char message_line[MSG_LENGTH];


// Takes in input a ptr to a memory space containing the message,
// reads and store the line into **** and the return the ptr to the new line
char* read_next_line(char* request_message){
	memset(message_line, 0, MSG_LENGTH); 
	char* walker = request_message;
	int cnt = 0;	

	//HTTP request's lines end with \r\n
	while(*walker != '\r' && *(walker+1) != '\n'){
		message_line[cnt++] = *walker;
		walker++;
	}
	
	//skip \r\n and return new line ptr
	return walker + 2;
}

void header_parser(char* request_message, struct request_header* request_ptr){
	//char word[MSG_LENGTH / 8];

	do{
		request_message = read_next_line(request_message);
		printf("%s\n", message_line);	
	}while(*request_message != '\r' && *(request_message+1) != '\n');


	printf("%zd\n", request_ptr->content_length);
}
