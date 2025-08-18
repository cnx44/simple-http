#include "net.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Open the socket, set socket options and return the server file descriptor 
int open_server_socket(struct sockaddr_in* address){
	//Open file descriptor via socket on ipv4, return -1 on error	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd < 0){
		perror("Error on socket opening");
		return -1; 
	}


	//port. Linux Kenrel distribute connections on the different socket
	//binded to the port
	int yes = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))){
		perror("Error on setsocketop SO_REUSEADDR");
		close(server_fd);
		return -1;
	}
	//SO_REUSEPORT allow to bind on port in TIME_WAITING, fast reboot 
	//without waiting for time out timer
	//SO_REUSEADDR allow multiple process/thread to run on the same 
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes))){
		perror("Error on setsockopt SO_REUSEPORT");
		close(server_fd);
		return -1;
	}

	//Bind port
    if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    //Set socket in listening mode
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }
	
	return server_fd;
}


//Look for waiting connections in queue, retry on error interrupt (EINTR)
//and connection aborted (ECONNABORTED), return client_fd to the worker
int accept_connection(int server_fd, struct sockaddr_in address){
	socklen_t addrlen;
	int client_fd;

	while(1){
		addrlen = sizeof(address);
		client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if(client_fd >= 0) return client_fd; //if read correct socker return fd
		if(errno == EINTR || errno == ECONNABORTED) continue; //on error try a new one
		return -1; //on other error return -1 "fail"
	}

}

ssize_t read_socket(int client_fd, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0){
		errno = EINVAL; //invalid arguments
		return -1; 
	}
	
	//Guard loop, on syscal while reading retry 
	while(1){
		size_t read_size = buffer_size - 1;
		ssize_t n = read(client_fd, buffer, read_size);
		
		if(n >= 0){ //Appen terminator char to ensure correct behaviour
			buffer[n] = '\0';
			return n;
		}

		if(errno == EINTR) continue; //interrupt error
		return -1;
	}
}

ssize_t write_socket(int client_fd, char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0)
        return -1;

    ssize_t n = write(client_fd, buffer, buffer_size);
    if (n < 0) 
        return -1;

    return n;
}

void close_connection(int client_fd){
	close(client_fd);
} 

