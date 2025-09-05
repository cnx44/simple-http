#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "net.h"
#include "http_parse.h"
#include "http_resp.h"

void enviroment_parser(struct environment_var* environment, char* path){
	FILE* file_ptr;
	char line_buffer[LINE_SIZE];
	char* word_buffer;
	int word_len;
	char delim[] = " \t";
		
	file_ptr = fopen(path, "r");
	if(!file_ptr){
		fprintf(stderr, "Error on conf file opening\n");
		exit(EXIT_FAILURE);
	}

	while(fgets(line_buffer, LINE_SIZE, file_ptr)){
		word_buffer = strtok(line_buffer, delim);	
		
		if(!strcmp(word_buffer, "index:")){
			word_buffer = strtok(NULL, delim); 
			word_len = strlen(word_buffer) + 1;

			environment->index = malloc(word_len * sizeof(char));
			if(!environment->index){
				fprintf(stderr, "Couldn't allocate memory for env-var");
				exit(EXIT_FAILURE);	
			}

			strcpy(environment->index, word_buffer);
			environment->index[word_len] = '\0';
		}else if(!strcmp(word_buffer, "favico:")){
			word_buffer = strtok(NULL, delim); 
			word_len = strlen(word_buffer) + 1;

			environment->fav_ico= malloc(word_len * sizeof(char));
			if(!environment->fav_ico){
				fprintf(stderr, "Couldn't allocate memory for env-var");
				exit(EXIT_FAILURE);	
			}

			strcpy(environment->fav_ico, word_buffer);
			environment->fav_ico[word_len] = '\0';
		}else if(!strcmp(word_buffer, "pages:")){
			word_buffer = strtok(NULL, delim); 
			word_len = strlen(word_buffer) + 1;

			environment->pages_folder= malloc(word_len * sizeof(char));
			if(!environment->pages_folder){
				fprintf(stderr, "Couldn't allocate memory for env-var");
				exit(EXIT_FAILURE);	
			}

			strcpy(environment->pages_folder, word_buffer);
			environment->pages_folder[word_len] = '\0';
		}else if(!strcmp(word_buffer, "error_pages:")){
			word_buffer = strtok(NULL, delim); 
			word_len = strlen(word_buffer) + 1;

			environment->error_pages = malloc(word_len * sizeof(char));
			if(!environment->error_pages){
				fprintf(stderr, "Couldn't allocate memory for env-var");
				exit(EXIT_FAILURE);	
			}

			strcpy(environment->error_pages, word_buffer);
			environment->error_pages[word_len] = '\0';
		}
		
	}
} 

int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr, "Please provide a conf file\n");
		exit(EXIT_FAILURE);
	}
	
	struct environment_var env;
	enviroment_parser(&env, argv[1]);

	int cnt = 0;
	
	struct sockaddr_in address;

	address.sin_family = AF_INET;			//AF_INET is used for ipv4 (AF_INET6 for ipv6)
    address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0.0.0.0, listening on all interfaces
    address.sin_port = htons(PORT);			//Set listening port to 8080 (PORT)

	int server_fd = open_server_socket(&address);
	
	while(1){ 	
		char buffer[MESSAGE_SIZE] = {0};
		int client_fd = accept_connection(server_fd, address);

		int read_byte = read_socket(client_fd, buffer, MESSAGE_SIZE);
		if(read_byte == -1) return EXIT_FAILURE;
		
		struct request_header* header = malloc(sizeof(struct request_header));
		if(!header) exit(EXIT_FAILURE);

		header_parser(buffer, header);
		
		char msg[32];
		sprintf(msg, "counter number: %d", cnt);
		message_sender(client_fd, msg, header);
		
		printf("%s\n", buffer);

		close_connection(client_fd, 1000);		//TODO: Handle return value
		cnt++;
	}	

	return EXIT_SUCCESS; 
}
