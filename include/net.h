#ifndef NET_H
#define NET_H

#include <arpa/inet.h>  

#define PORT 8080
#define MESSAGE_SIZE 8192

int open_server_socket(void);

int accept_connection(int server_fd, struct sockaddr_in address);

ssize_t read_socket(int client_fd, char* buffer, size_t buffer_size);

ssize_t write_socket(int client_fd, char* buffer, size_t buffer_size);

ssize_t write_socket(int client_fd, char *buffer, size_t buffer_size);

#endif
