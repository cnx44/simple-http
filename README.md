# C11 HTTP Server

A lightweight, custom-built HTTP server written from scratch in C. This project was developed to gain a deep, hands-on understanding of network programming, the TCP/IP stack, and the internal mechanics of the HTTP protocol.

## Overview

This server implementation bypasses high-level abstractions to interact directly with the operating system's networking APIs. It handles socket lifecycle management, parses raw HTTP request buffers, and serves static content based on configurable rules.

## Key Features

*   **Socket Management**: Direct implementation of the socket, bind, listen, and accept lifecycle. Utilizes SO\_REUSEADDR and SO\_REUSEPORT for robust connection handling and rapid restarts.
*   **Custom HTTP Parser**: A handmade parser that processes raw byte streams to extract HTTP methods (GET), headers, paths, and query parameters without relying on external HTTP libraries.
*   **Configurable Environment**: Bootstraps using a custom configuration parser (src/server.c) that reads runtime settings (root directory, index file, error pages) from a config file.
*   **Static File Serving**: Efficiently reads and serves HTML, CSS, and image assets from the websources directory.
*   **Error Handling**: Custom error pages (e.g., 404) defined via configuration.

## Build & Run

### Prerequisites
*   GCC (GNU Compiler Collection)
*   Make
*   Linux/Unix-like environment (macOS/Darwin supported)

### Compilation
The project uses a standard Makefile for compilation.

```bash
# Build the project (Release mode: -O2 optimizations)
make

# Build for debugging (Debug mode: -g -O0)
make debug

# Clean build artifacts
make clean
```

### Running the Server
The server requires a configuration file path as an argument.

```bash
./http_server conf/server.conf
```
*(Note: Ensure your configuration file points to valid paths for your static assets)*

## Project Structure

*   `src/`: Source code implementing the core server logic, networking, and parsing.
    *   `server.c`: Main entry point and connection loop.
    *   `net.c`: Low-level socket wrappers and I/O.
    *   `http_parse.c`: HTTP request parsing logic.
    *   `http_resp.c`: Response generation.
*   `include/`: Header files defining structures and interfaces.
*   `websources/`: Directory containing static assets (HTML, images) served by the application.
*   `conf/`: Configuration files.

## Future Improvements

*   **Concurrency**: Implementation of a thread pool or epoll/kqueue event loop to handle multiple simultaneous connections non-blocking.
*   **HTTP Methods**: Support for POST/PUT requests and dynamic content handling (CGI).
*   **Security**: Implementation of basic security headers and input sanitization.
