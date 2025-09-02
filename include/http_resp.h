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

#ifndef HTTP_RESP_H
#define HTTP_RESP_H

#include "http_parse.h"
#include "net.h"
#include "string.h"
#include "stdlib.h"

char* header_builder(struct request_header* request_hd);

void message_sender(int client_fd, char* message, struct request_header* request_hd);

#endif
