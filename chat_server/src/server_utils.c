#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_winsock_error(int error_code) {
    char *msg = NULL;
    DWORD dwSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, (LPSTR)&msg, 0, NULL);
    if (dwSize) {
        printf("[ERROR] WinSock error: %s\n", msg);
        LocalFree(msg);
    } else {
        printf("[ERROR] Unexpected error occurred.\n");
    }
}

void send_accept_response(SOCKET socket) {
    const char *response = "OK";
    int bytes_sent = send(socket, response, strlen(response) + 1, 0);
    if (bytes_sent == SOCKET_ERROR) {
        print_winsock_error(WSAGetLastError());
    }
}

void send_reject_response(SOCKET socket) {
    const char *response = "NOT";
    int bytes_sent = send(socket, response, strlen(response) + 1, 0);
    if (bytes_sent == SOCKET_ERROR ) {
        print_winsock_error(WSAGetLastError());
    }
}