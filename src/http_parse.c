#include "http_parse.h"

char message_line[MSG_LENGTH];


// Takes in input a ptr to a memory space containing the message,
// reads and store the line into **** and the return the ptr to the new line
char* read_next_line(char* request_message){
	memset(message_line, 0, MSG_LENGTH); 
	char* walker = request_message;
	int cnt = 0;	

	//HTTP request's lines end with \r\n
	while(cnt < MSG_LENGTH && *walker != '\0'){
		//Use only \r as non canoncal line terminator
		if(*walker == '\r') break;
		
		message_line[cnt++] = *walker++;
	}
	message_line[cnt] = '\0'; //Append str terminator

	//return next line if we get CR LF
	if(*walker == '\r' && *(walker+1) == '\n') return walker + 2;

	return walker; //return ptr if we exit the while for cnt
}

// Lower casing the char byt xoring  
static void to_lowercase(char* word, int len){
	for(int i = 0; i < len; i++){
		if(word[i]>= 65	&& word[i] <= 90){
			word[i] = word[i] ^ 32;
		}else if(word[i] == '\0') return;
	}
}


// in word we will save the next word taken from str_ptr, the max length 
// in word is word_len. Return next word ptr if we read the full word
// next unread byte else
char* read_word(char* word, char* str_ptr, int word_len){
	if(!word || !str_ptr) return str_ptr;
	int cnt = 0;
	
	memset(word, 0, word_len);
	
	while(*str_ptr == ' ') str_ptr++;

	while((*str_ptr != ' ' && *str_ptr != 0) && cnt < word_len-1){
		word[cnt] = *str_ptr;
		cnt++;
		str_ptr++;
	}
	word[cnt] = '\0';
	to_lowercase(word, cnt);	

	while(*str_ptr == ' ') str_ptr++;

	return str_ptr;
}

// Populate the request_target, path, query and version fields in request_header struct
void header_get_parser(char* walker, struct request_header* request_ptr){
	char word[WRD_LENGTH] = {0};
	walker = read_word(word, walker, sizeof(word));
	size_t path_size = strlen(word) + 1;
	request_ptr->request_target = malloc(path_size * sizeof(char));
	strcpy(request_ptr->request_target, word);

	size_t query_separator = 0;
	while(query_separator < path_size && word[query_separator] != '?') query_separator++;	

	if(query_separator < path_size && word[query_separator] == '?'){
		request_ptr->path = malloc((query_separator+1)*sizeof(char));
		memcpy(request_ptr->path, word, query_separator);
		request_ptr->path[query_separator] = '\0'; 
		
		size_t query_start = query_separator + 1;
		size_t query_len	= path_size - query_start;
		request_ptr->query = malloc((query_len)*sizeof(char));
		memcpy(request_ptr->query, word + query_start, query_len);
		//request_ptr->query should catch the \0 from word
		//if(!request_ptr->path || !request_ptr->query) //TODO: handle memroy allocation failure
	}else{	// no query
		request_ptr->path = malloc(path_size * sizeof(char));
		request_ptr->query = NULL; 
		strcpy(request_ptr->path, request_ptr->request_target);
	}
	

	walker = read_word(word, walker, sizeof(word));
	if(strcmp(word, "http/1.1") == 0){
		request_ptr->version = HTTP_1_1;
	}else if(strcmp(word, "http/1.0") == 0){
		request_ptr->version = HTTP_1_0;
	}else if(strcmp(word, "http/2.0") == 0){
		request_ptr->version = HTTP_2_0;
	}else{
		request_ptr->version = HTTP_UNSUPPORTED;
	}
}

// Populate the host field in request_header struct
void header_host_parser(char* walker, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8] = {0};
	walker = read_word(word, walker, sizeof(word));
	size_t host_len = strlen(word) + 1;
	request_ptr->host = malloc(host_len * sizeof(char));
	//TODO: handle error on malloc
	strcpy(request_ptr->host, word);
}


//Populate the connection field in request_header struct
void header_connection_parser(char* walker, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8] = {0};
	walker = read_word(word, walker, sizeof(word));
	if(strcmp(word, "keep-alive") == 0){
	request_ptr->connection = KEEP_ALIVE;
	}else{
	request_ptr->connection = CLOSE;
	}
}

// Read request_message string and populate struct request_header
void header_parser(char* request_message, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8];
	
    while (*request_message != '\0') {
        // Read the current line and update the pointer
        request_message = read_next_line(request_message);

        // Empty linea means header's end
        if (message_line[0] == '\0') {
            break;
        }

        // Read word by word
        char *walker = message_line;
        while (*walker != '\0') {
            walker = read_word(word, walker, sizeof(word));

			if(strcmp(word, "get") == 0){
				header_get_parser(walker, request_ptr);	
			}else if(strcmp(word, "host:") == 0){
				header_host_parser(walker, request_ptr);
			}else if(strcmp(word, "connection:") == 0){
				header_connection_parser(walker, request_ptr);
			}
		}
    }
}


/* HEADER PRINTER DEBUG
printf("HTTP version: %d\n", request_ptr->version);
printf("Host address: %s\n", request_ptr->host);
if(request_ptr->connection == KEEP_ALIVE){
printf("Connection: KEEP_ALIVE\n");
}else{
printf("Connection: CLOSE\n");
}


printf("Request path: %s\n", request_ptr->path);
if(request_ptr->query) printf("Request query: %s\n", request_ptr->query);
*/
