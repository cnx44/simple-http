/* Current problems for future improvements
 *
 * DoS from server shutdown
 * close_connection() also closes server_fd.
 * After the first connection, the server dies.
 * An attacker sends 1 request -> service goes down.
 *
 * Tiny backlog (1) -> DoS/SYN flood
 * listen(server_fd, 1) creates a very small accept queue.
 * Just a few half-open connections are enough to saturate it
 * (SYN flood / half-connections) and block new clients.
 *
 * Slowloris / slow connections
 * Blocking sockets, no timeout/keep-alive/limits:
 * a client can drip-feed the header and occupy the only worker flow.
 * With a single-thread/simple loop server, this is an easy DoS.
 *
 * Fragile accept (shutdown and exit)
 * In accept_connection(), on error you close the server
 * and call exit(EXIT_FAILURE).
 * A transient error (EINTR, resource temporarily unavailable)
 * crashes everything -> remote DoS.
 *
 * Wrong check on accept
 * You check client_fd < 1. accept() may return 0 (valid if fd 0 is unused);
 * you would treat it as an error and kill the server.
 * Exploitable under certain conditions.
 *
 * Missing SO_REUSEADDR/SO_REUSEPORT
 * Hard to restart (TIME_WAIT).
 * An attacker can force many connections and prevent
 * you from restarting quickly on the same port.
 *
 * Overly broad binding
 * INADDR_ANY exposes on all interfaces.
 * If you wanted only local, now it’s public.
 * Risk of scans, bots, automated exploits.
 *
 * sockaddr_in struct not zeroed
 * You don’t memset(&address, 0, sizeof address).
 * Garbage fields (e.g. sin_zero) shouldn’t matter,
 * but it’s bad practice and can cause weird behavior.
 *
 * No SIGPIPE protection
 * If the client closes and you call write(),
 * you may get SIGPIPE and the process terminates.
 * Exploitable by opening/closing rapidly.
 *
 * Partial writes and memory leaks
 * write_socket() passes buffer_size to write(), not the actual length.
 * If buffer_size > strlen, you might send uninitialized data (info leak).
 * No handling of short writes: truncated response can be manipulated.
 *
 * Partial reads / incomplete protocol
 * read_socket() reads only once. HTTP can arrive in multiple chunks.
 * You might parse truncated or mixed requests,
 * leading to unpredictable behavior (request smuggling/desync
 * if later put behind a proxy).
 *
 * No limit on request size
 * Even if you avoid overflow, you can suffer resource exhaustion
 * (huge headers/bodies) or internal buffering that blocks the thread.
 *
 * No TLS
 * Traffic is cleartext: sniffing, MITM, theft of cookies/credentials.
 *
 * No isolation
 * If running as root and the process is exploited,
 * the host is compromised (no chroot/pledge/seccomp/priv-drop).
 *
 * IPv4-only
 * Not an attack, but limits scope;
 * closing IPv6 won’t save you from IPv4 scans.
 */

#include "net.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Open the socket and return the server file descriptor */
int open_server_socket(){
	int server_fd;
	struct sockaddr_in address;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)	return server_fd;

	address.sin_family = AF_INET;			//AF_INET is used for ipv4 (AF_INET6 for ipv6)
    address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0.0.0.0, listening on all interfaces
    address.sin_port = htons(PORT);			//Set listening port to 8080 (PORT)
	
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
		perror("bind failed");
		return -1;
	}	

	if (listen(server_fd, 1) < 0) {
        perror("listen failed");
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

void close_connection(int client_fd, int server_fd){
	close(client_fd);
	close(server_fd);
} 
