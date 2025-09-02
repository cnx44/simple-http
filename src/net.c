#include "net.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <poll.h>

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
    if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0){
        perror("bind");
        close(server_fd);
        return -1;
    }

    //Set socket in listening mode
    if (listen(server_fd, SOMAXCONN) < 0){
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

//Read buffer_size - 1 bytes from client's file descriptor and store them into
//char* buffer, on syscal interrupt error retry, return ssize_t read bytes 
//or -1 on error
ssize_t read_socket(int client_fd, char* buffer, size_t buffer_size){
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


// Write to client_fd the content of buffer and return the ssize_t with the 
// number of bytes written, on severe failuer return -1 as per nothing written
ssize_t write_socket(int client_fd, const void* buffer, size_t message_size){
	if(!buffer || message_size == 0 || message_size > SSIZE_MAX){
		errno = EINVAL; //Invalid arguments
		return -1;	
	}
	
	const unsigned char* remaining_message = buffer;
	size_t left = message_size;
	ssize_t total = 0;

	while(1){
		ssize_t written_bytes = write(client_fd, remaining_message, left);
		
		//Write what it can and eventually return
		if(written_bytes > 0){
			remaining_message += written_bytes;
			left -= (size_t) written_bytes;
			total += written_bytes;
			if(left == 0) return total;
		}
		
		//On interrupt error try to write the rest of the message	
		if(written_bytes < 0 && errno == EINTR) continue;
		//EAGAIN in modern POSIX is an alias for EWOULDBLOCK
		//if the action would cause an interrupt we skip the rest
		if(written_bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)){
			if(total > 0) return total;
			return -1;
		}

		return -1;
	}

}

//Shutdown the writing side, then keep reading until the timeout > 0
//or the client close the connection. Returns 0 on closing fd correctly
// -1 else
int close_connection(int client_fd, int drain_timouts_ms){
	shutdown(client_fd, SHUT_WR); //Close the Writing-channel on the socket
 

	if(drain_timouts_ms > 0){
	//It notify us when we can read client_fd without blocking
	struct pollfd pool_fd = {.fd = client_fd, .events = POLL_IN};
	while(1){
	int poll_result = poll(&pool_fd, 1, drain_timouts_ms);
	if(poll_result <= 0) break; //Timeout or error
	char buffer[MESSAGE_SIZE];	
	ssize_t read_byte = read(client_fd, buffer, sizeof(buffer));
	if(read_byte <= 0) break; //EOF or error
	}
	}
	return close(client_fd); //return 0 on all right, -1 else
} 

