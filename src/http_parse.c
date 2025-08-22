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

	while(*str_ptr == ' ') str_ptr++;

	return str_ptr;
}

void header_parser(char* request_message, struct request_header* request_ptr){
	char word[MSG_LENGTH / 8];
	

    while (*request_message != '\0') {
        // Legge una riga e la mette in message_line
        request_message = read_next_line(request_message);

        // Se la riga è vuota, hai trovato la separazione headers/body
        if (message_line[0] == '\0') {
            printf("Riga vuota trovata -> fine headers\n");
            break;
        }

        printf("Riga: \"%s\"\n", message_line);

        // Estrai parole dalla riga
        char *walker = message_line;
        while (*walker != '\0') {
            walker = read_word(word, walker, sizeof(word));
            if (word[0] != '\0') {
                printf("  parola: \"%s\"\n", word);
            }
        }
    }


	printf("%zd\n", request_ptr->content_length);
}
