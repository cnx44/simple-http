/*
-----------200 OK JS
-----------501 NOT IMPL
-----------500 INTERNAL ERR
-----------404 NOT FOUND
Content-Type: text/html; charset=UTF-8
Content-Length: 0
Connection: close

*/
#include "http_resp.h"

const char OK_RESP[]		= "HTTP/1.1 200 OK\r\n";
const char INTERNAL_ERR[]	= "HTTP/1.1 500 Internal Server Error\r\n";
const char NOT_IMPL[]		= "HTTP/1.1 501 Not Implemented\r\n";
const char NOT_FOUND[]		= "HTTP/1.1 404 Not Found\r\n";

const char CLOSE_CONN[]		= "Connection: close\r\n";

void date_builder(char* date){
	time_t curr_time = time(NULL);
	struct tm gmt;

	gmtime_r(&curr_time, &gmt);

	strftime(date, STR_BUFFER_LEN, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n",&gmt);
}

char* message_builder(struct request_header* request_hd){
	bool found = true;
	char* rsp_msg = malloc(MSG_BUFFER_LEN * sizeof(char));
	char* date = malloc(STR_BUFFER_LEN * sizeof(char)); 
	char* resource_path = malloc(STR_BUFFER_LEN * sizeof(char));
	char* resource_cont = malloc(STR_BUFFER_LEN * sizeof(char));	
	FILE* file_ptr;

	if(!strcmp(request_hd->path, "/")){ 
		strcpy(resource_path, env.index); 
	}else if(!strcmp(request_hd->path, "/favicon.ico")){
		strcpy(resource_path, env.fav_ico); 
	}else{
		strcpy(resource_path, env.pages_folder);
		strcat(resource_path, request_hd->path);
	}
	
	file_ptr = fopen(resource_path, "rb");
	if(!file_ptr){		//404 ERROR HANDLER 
		found = false;
		strcpy(resource_path, env.error_pages);
		strcat(resource_path, "/404.html");
		file_ptr = fopen(resource_path, "rb");
	}



	size_t nread = fread(resource_cont, 1, STR_BUFFER_LEN, file_ptr);
	resource_cont[nread] = '\0';
	fclose(file_ptr);	

	date_builder(date);
	
	if(found){
		strcpy(rsp_msg, OK_RESP);
	}else{
		strcpy(rsp_msg, NOT_FOUND);
	}
	strcat(rsp_msg, date);
	strcat(rsp_msg, CLOSE_CONN);

	strcat(rsp_msg, "\r\n");
	strcat(rsp_msg, resource_cont);


	free(resource_path);
	free(resource_cont);
	free(date);
	return rsp_msg;
}
