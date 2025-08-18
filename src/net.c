/*
TODO: HTTP/1.1 response headers (basic server)

Response structure:
    HTTP/1.1 <status-code> <reason-phrase>\r\n
    <header-name>: <value>\r\n
    ...
    \r\n
    [body]

MINIMUM headers for compatibility:
    - Content-Length: <bytes>   (or Transfer-Encoding: chunked)
    - Content-Type: <mime>      (e.g. text/html, text/plain, application/json)
    - Connection: close         (simpler; or keep-alive if supported)
    - Date: <RFC1123 GMT date>  (strongly recommended)
    - Server: <name/version>    (for debugging/compatibility)

CONDITIONAL / CONTEXT headers:
    - Location: <url>           (for 3xx redirects)
    - Cache-Control, ETag, Last-Modified (for caching/conditional requests)
    - Content-Encoding: gzip    (if compression is implemented)
    - Allow: GET, POST          (for 405 Method Not Allowed)
    - WWW-Authenticate: ...     (for 401 Unauthorized)

Notes:
    - In HTTP/1.1 connections are persistent by default if you do NOT use
      Content-Length or chunked encoding, you must send "Connection: close"
      and close the socket after the response.
    - Some status codes (1xx, 204, 304, and responses to HEAD) MUST NOT have a body.
*/
#include "net.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Open the socket and return the server file descriptor */
int open_server_socket(struct sockaddr_in* address){
	//Open file descriptor via socket on ipv4, return -1 on error	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd < 0) return -1;

	int yes = 1;
	//Enable binding on port even if it is in TIME_WAIT (fast restart)
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))){
		perror("Error on setsocketop SO_REUSEADDR");
		close(server_fd);
		return -1;
	}
	//Enable multiple process/threads to bind the same port	
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


int accept_connection(int server_fd, struct sockaddr_in address){
	socklen_t addrlen = sizeof(address);
	int	client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
	if (client_fd < 1) {
		perror("accept failed");
		return -1;
	}
	return client_fd;
}

ssize_t read_socket(int client_fd, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return -1;

    ssize_t n = read(client_fd, buffer, buffer_size - 1);
    if (n < 0) return -1; 

    buffer[n] = '\0'; 
    return n;
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

