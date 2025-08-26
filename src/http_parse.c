/*
EXAMPLE REQUEST

Connection: keep-alive
Cache-Control: max-age=0
sec-ch-ua: "Chromium";v="139", "Not;A=Brand";v="99"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "macOS"
Upgrade-Insecure-Requests: 1
DNT: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/139.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,;q=0.8,application/signed-exchange;v=b3;q=0.7
Sec-Fetch-Site: cross-site
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: it-IT,it;q=0.9,en-US;q=0.8,en;q=0.7
 */


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


void header_get_parser(char* walker, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8] = {0};
	walker = read_word(word, walker, sizeof(word));
	size_t path_size = strlen(word) + 1;
	request_ptr->request_target = malloc(path_size * sizeof(char));
	strcpy(request_ptr->request_target, word);

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

void header_host_parser(char* walker, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8] = {0};
	walker = read_word(word, walker, sizeof(word));
	size_t host_len = strlen(word) + 1;
	request_ptr->host = malloc(host_len * sizeof(char));
	//TODO: handle error on malloc
	strcpy(request_ptr->host, word);
}


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

	printf("HTTP version: %d\n", request_ptr->version);
	printf("Host address: %s\n", request_ptr->host);
	if(request_ptr->connection == KEEP_ALIVE){
		printf("Connection: KEEP_ALIVE\n");
	}else{
		printf("Connection: CLOSE\n");
	}
}
