**TCP HTTP Server on Windows**

---

```c
#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
```

1. `#include <winsock2.h>`

   * Imports the Winsock 2 API definitions. Required for socket functions on Windows.
2. `#include <Windows.h>`

   * Includes core Windows definitions (e.g., `ZeroMemory`).
3. `#include <stdio.h>`

   * Provides input/output functions like `printf` and `fprintf`.
4. `#pragma comment(lib, "Ws2_32.lib")`

   * Instructs the linker to link against the Winsock library (`Ws2_32.lib`). Ensures socket functions resolve.

---

```c
int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }
```

5. `int main() {`

   * Entry point of the program.
6. `WSADATA wsadata;`

   * Structure to receive details of the Windows Sockets implementation.
7. `WSAStartup(MAKEWORD(2, 2), &wsadata)`

   * Initializes Winsock version 2.2. Fills `wsadata` on success.
8. `if (...) != 0)`

   * Checks for errors in initialization. Non-zero return means failure.
9. `fprintf(stderr, "WSAStartup failed\n");`

   * Outputs an error message to standard error.
10. `return 1;`

* Exits program with error code 1.

---

```c
    SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
```

11. `SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);`

    * Creates a TCP socket (IPv4). `AF_INET` = IPv4, `SOCK_STREAM` = TCP.
12. `if (listener == INVALID_SOCKET)`

    * Checks if socket creation failed.
13. `WSAGetLastError()`

    * Retrieves the last Winsock error code.
14. `WSACleanup();`

    * Cleans up Winsock resources before exiting.

---

```c
    struct sockaddr_in addr;
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(3000);
```

15. `struct sockaddr_in addr;`

    * Defines an IPv4 address structure for binding.
16. `ZeroMemory(&addr, sizeof(addr));`

    * Clears the `addr` structure to zero.
17. `addr.sin_family = AF_INET;`

    * Sets address family to IPv4.
18. `addr.sin_addr.s_addr = htonl(INADDR_ANY);`

    * Binds to all available interfaces. `htonl` converts to network byte order.
19. `addr.sin_port = htons(3000);`

    * Sets port to 3000. `htons` converts to network byte order.

---

```c
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
```

20. `bind(...)`

    * Associates the socket with the local address and port.
21. On error, prints the error code, closes the socket, cleans up Winsock, and exits.

---

```c
    if (listen(listener, 5) == SOCKET_ERROR) {
        fprintf(stderr, "listen() failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    printf("Listening on port 3000...\n");
```

22. `listen(listener, 5)`

    * Marks the socket as a passive (listening) socket. The `5` is the backlog queue size.
23. On failure, cleans up and exits as before.
24. `printf("Listening on port 3000...\n");`

    * Informs the user that the server is ready.

---

```c
    SOCKET client = accept(listener, NULL, NULL);
    if (client == INVALID_SOCKET) {
        fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
    }
    else {
        char request[1024] = { 0 };
        int r = recv(client, request, sizeof(request) - 1, 0);
```

25. `accept(listener, NULL, NULL)`

    * Waits for an incoming connection. Returns a new socket for the client.
26. Checks for errors; logs if `INVALID_SOCKET`.
27. On success, declares a buffer for the HTTP request and receives data via `recv`.
28. `recv(..., sizeof(request)-1, 0)`

    * Reads up to 1023 bytes. The last byte reserved for null-termination.

---

```c
        if (r > 0 && memcmp(request, "GET / ", 6) == 0) {
            FILE* f = fopen("index.html", "rb");
            if (f) {
                char body[4096];
                size_t body_len = fread(body, 1, sizeof(body), f);
                fclose(f);

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
        }
```

29. `if (r > 0 && memcmp(request, "GET / ", 6) == 0)`

    * Checks that data was received and the request starts with `"GET / `".
30. `fopen("index.html", "rb")`

    * Opens `index.html` in binary mode for reading.
31. Reads file into the `body` buffer and captures length.
32. Constructs an HTTP response header containing status, content type, content length, and connection directive.
33. Uses `snprintf` to safely format the header string.
34. `send(...)` calls to transmit the header and body to the client.

---

```c
        closesocket(client);
    }

    closesocket(listener);
    WSACleanup();
    return 0;
}
```

35. `closesocket(client);`

    * Closes the client connection.
36. `closesocket(listener);`

    * Shuts down the listening socket.
37. `WSACleanup();`

    * Releases Winsock resources.
38. `return 0;`

    * Exits the program indicating success.

---

**Usage**

1. Place `index.html` in the same directory as the compiled executable.
2. Compile with a C compiler on Windows (e.g., MSVC).
3. Run the program; open a browser at `http://localhost:3000/`.

**Notes & Extensions**

* This example only handles a single request and exits.
* For production, consider looping `accept` and adding multithreading.
* Always validate and sanitize input for security.

---
