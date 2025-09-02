#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "net.h"
#include "http_parse.h"
#include "http_resp.h"

#define PORT 8080
#define MESSAGE_SIZE 8192


int main(){
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

		close_connection(client_fd, 1000);		//TODO: Handle return value
		cnt++;
	}	

	return EXIT_SUCCESS; 
}
