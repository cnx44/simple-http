#include "net.h"

#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>  

/* Open the socket and return the server file descriptor */
int open_server_socket(){
	int server_fd;
	struct sockaddr_in address;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)	return server_fd;

	address.sin_family = AF_INET;			//AF_INET is used for ipv4 (AF_INET6 for ipv6)
    address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0.0.0.0, listening on all interfaces
    address.sin_port = htons(PORT);			//Set listening port to 8080 (PORT)
	
	//Binding file descriptor to the couple <ip,port> in address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
		return -1;
		perror("bind failed");
	}	

    //Put the socket in passive mode, waiting for a client
	if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
		return -1;
	}
	

    printf("Server listening on http://127.0.0.1:%d\n", PORT);

	return server_fd;
}


/*int main(int argc, char** argv){



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
}*/
