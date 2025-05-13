#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }
    SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    struct sockaddr_in addr;
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(3000);
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    if (listen(listener, 5) == SOCKET_ERROR) {
        fprintf(stderr, "listen() failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    printf("Listening on port 3000...\n");
    SOCKET client = accept(listener, NULL, NULL);
    if (client == INVALID_SOCKET) {
        fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
    }
    else {
        char request[1024] = { 0 };
        int r = recv(client, request, sizeof(request) - 1, 0);
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
        closesocket(client);
    }

    closesocket(listener);
    WSACleanup();
    return 0;
}
