## Simple Winsock HTTP Server in C - Beginner Documentation

### Header Files and Pragmas

```c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "Ws2_32.lib")
```

* **\<winsock2.h>**: Core Winsock 2.0 API for networking on Windows.
* **\<ws2tcpip.h>**: Adds modern networking helpers (IPv6, etc.).
* **\<stdio.h>**: Standard I/O functions (e.g., printf, fopen).
* **\<stdlib.h>**: Utilities like memory management, exit control.
* **\<string.h>**: String functions like memcpy.
* **#pragma comment**: Links the Winsock library at build time.

---

### Initializing Winsock

```c
WSADATA wsaData;
if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    fprintf(stderr, "WSAStartup failed\n");
    return 1;
}
```

* Initializes the Winsock library. Must be done before any networking.
* `MAKEWORD(2, 2)` asks for version 2.2.

---

### Creating the Listening Socket

```c
SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
if (listener == INVALID_SOCKET) {
    fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
}
```

* Creates a TCP socket using IPv4.
* Returns INVALID\_SOCKET on failure.

---

### Binding the Socket

```c
struct sockaddr_in addr;
ZeroMemory(&addr, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(3000);
```

* `INADDR_ANY`: Bind to all available interfaces.
* `htons(3000)`: Host to network byte order for port 3000.

```c
if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
    fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
    closesocket(listener);
    WSACleanup();
    return 1;
}
```

* Binds the socket to IP and port.

---

### Listening for Connections

```c
if (listen(listener, 5) == SOCKET_ERROR) {
    fprintf(stderr, "listen() failed: %d\n", WSAGetLastError());
    closesocket(listener);
    WSACleanup();
    return 1;
}
printf("Listening on port 3000...\n");
```

* Enables the socket to listen for incoming TCP connections.

---

### Accepting a Client

```c
SOCKET client = accept(listener, NULL, NULL);
if (client == INVALID_SOCKET) {
    fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
}
```

* Accepts a client. Returns a new socket for that client.

---

### Receiving an HTTP GET Request

```c
char request[1024] = { 0 };
int r = recv(client, request, sizeof(request) - 1, 0);
if (r > 0 && memcmp(request, "GET / ", 6) == 0) {
    // Process request
}
```

* Reads data from client.
* Checks if it's a simple "GET / " HTTP request.

---

### Reading HTML File

```c
FILE* f = fopen("index.html", "rb");
if (f) {
    char body[4096];
    size_t body_len = fread(body, 1, sizeof(body), f);
    fclose(f);
```

* Opens the `index.html` file.
* Reads up to 4096 bytes into `body`.

---

### Sending HTTP Response

```c
    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len
    );
    send(client, header, header_len, 0);
    send(client, body, (int)body_len, 0);
}
```

* Sends proper HTTP headers and file content to the client.
* Terminates headers with an extra `\r\n`.

---

### Cleanup

```c
closesocket(client);
closesocket(listener);
WSACleanup();
return 0;
```

* Closes both client and listener sockets.
* Frees Winsock resources.

---

### Summary: Winsock Server Steps

1. `WSAStartup()` - Initialize networking.
2. `socket()` - Create TCP socket.
3. `bind()` - Attach socket to local IP\:Port.
4. `listen()` - Wait for connections.
5. `accept()` - Accept a client.
6. `recv()` - Receive HTTP request.
7. `fopen()` + `fread()` - Read HTML file.
8. `send()` - Send HTTP response.
9. `closesocket()` + `WSACleanup()` - Clean up.

Use this markdown doc as a quick reference for writing or debugging simple Winsock HTTP servers.

