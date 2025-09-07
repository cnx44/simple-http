#ifndef HTTP_RESP_H
#define HTTP_RESP_H

#include "http_parse.h"
#include "net.h"
#include "string.h"
#include "stdlib.h"
#include "server.h"
#include "time.h"
#include "stdbool.h"

#define STR_BUFFER_LEN 512
#define MSG_BUFFER_LEN 65536

enum content_type{
	TEXT_HTML,
	TEXT_CSS,
	
	APPLICATION_JS,

	IMAGE_PNG,
	IMAGE_JPEG,
	IMAGE_GIF,
	IMAGE_SVG_XML,
	IMAGE_X_ICON

};
char* message_builder(struct request_header* request_hd);

#endif
