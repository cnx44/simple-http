#include <stdlib.h>
#include <stdio.h>
#include "net.h"

#define PORT 8080
#define MESSAGE_SIZE 8192


int main(){
	int server_fd = open_server_socket();

	printf("server file descriptor %d", server_fd);
	return EXIT_SUCCESS;
}
