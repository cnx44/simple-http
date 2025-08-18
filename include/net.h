#ifndef NET_H
#define NET_H

#include <arpa/inet.h>  

#define PORT 8080
#define MESSAGE_SIZE 8192

int open_server_socket(struct sockaddr_in* address);

int accept_connection(int server_fd, struct sockaddr_in address);

ssize_t read_socket(int client_fd, char* buffer, size_t buffer_size);

ssize_t write_socket(int client_fd, const void* buffer, size_t message_siz);

int close_connection(int client_fd, int drain_timouts_ms);

#endif
