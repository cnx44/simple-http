#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>       // read, write, close
#include <arpa/inet.h>  

#define PORT 8080
#define MESSAGE_SIZE 8192

int main(int argc, char** argv){
	//fd stands for file descriptor which is part of the 
	//POSIX API to identify open file.
	//by defaul 0,1,2 are stdin stdout stderr in stdio.h
	int server_fd, client_fd;
	
	//part of netinet/in.h in linux.
	//It is used to hold the caller address
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	
	//used to hold the client message
	char buffer[MESSAGE_SIZE] = {0};
	//TODO: lazy load pages
	const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>Hello from C server!</h1></body></html>";
	

	//socket() is a syscall, it may fail on different scenareos
	//such as to much file opend or saturated sockets for the system
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {	//On failure fd < 0
        perror("Failure on opening Socket");
        exit(EXIT_FAILURE);
    }

	address.sin_family = AF_INET;			//AF_INET is used for ipv4 (AF_INET6 for ipv6)
    address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0.0.0.0, listening on all interfaces
    address.sin_port = htons(PORT);			//Set listening port to 8080 (PORT)
	
	//Binding file descriptor to the couple <ip,port> in address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //Put the socket in passive mode, waiting for a client
	if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on http://127.0.0.1:%d\n", PORT);

	//accept() dequeue from the requet queue (kernel level) and blocks
	//if none is present
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 1) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // POSIX API to handle file descriptors
	// read(fd, buf, count) read at most count byte from fd and write them into bff
    read(client_fd, buffer, sizeof(buffer) - 1);
    printf("Received request:\n%s\n", buffer);

    // put on client file descriptor the response
    write(client_fd, response, strlen(response));



    close(client_fd);
    close(server_fd);
	return EXIT_SUCCESS;
}
