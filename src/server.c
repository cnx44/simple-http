#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "net.h"

#define PORT 8080
#define MESSAGE_SIZE 8192


int main(){
	int cnt = 0;
	
	struct sockaddr_in address;

	address.sin_family = AF_INET;			//AF_INET is used for ipv4 (AF_INET6 for ipv6)
    address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0.0.0.0, listening on all interfaces
    address.sin_port = htons(PORT);			//Set listening port to 8080 (PORT)

	int server_fd = open_server_socket(&address);
	printf("server fd: %d\n", server_fd);
	
	while(1){ 	
		char buffer[MESSAGE_SIZE] = {0};
		int client_fd = accept_connection(server_fd, address);
		printf("client fd: %d\n", client_fd);

		int read_byte = read_socket(client_fd, buffer, MESSAGE_SIZE);
		if(read_byte == -1) return EXIT_FAILURE;
		printf("MESSAGE:\n%s", buffer);

		char ret_msg[512];

		
		snprintf(ret_msg, sizeof(ret_msg),
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n"
			"\r\n"
			"<html><body><h1>200 OK! cnt = %d</h1></body></html>",
			cnt);

		int written_byte = write_socket(client_fd, ret_msg, strlen(ret_msg));
		if(written_byte == -1) return EXIT_FAILURE;

		printf("MESSAGE: %s\nMESSAGE SIZE: %d\n", ret_msg, written_byte);
		cnt++;
	}	
	return EXIT_SUCCESS; }
