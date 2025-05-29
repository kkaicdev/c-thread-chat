#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include "../include/client_net.h"
#include "../include/client_utils.h"

int init_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

SOCKET create_and_connect_socket(struct sockaddr_in *server) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        handle_error("[ERROR] Error at creating socket", sock);
        closesocket(sock);
    }

    printf("[INFO] Connecting to server...\n");

    if (connect(sock, (struct sockaddr *)server, sizeof(*server)) == SOCKET_ERROR) {
        handle_error("[ERROR] Failed to connect to server", sock);
        closesocket(sock);
    }

    printf("[INFO] Connection established successfully.\n");
    return sock;
}

DWORD WINAPI receive_messages(LPVOID sock_ptr) {
    SOCKET sock = *(SOCKET *)sock_ptr;
    char buffer[BUFFER_SIZE];

    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            if (n == 0) {
                printf("\n[ERROR] Connection closed by the server. Server full or name already in use.\n");
                closesocket(sock);
            } else {
                handle_error("[ERROR] Error receiving message", sock);
                closesocket(sock);
            }
            break;
        }

        buffer[n] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';

        printf("\r[MESSAGE] %s\n", buffer);
        printf("Enter your message: ");
        fflush(stdout);
    }
    return 0;
}

int send_message(SOCKET sock, const char *message) {
    int total_sent = 0;
    int len = (int)strlen(message);

    while (total_sent < len) {
        int sent = send(sock, message + total_sent, len - total_sent, 0);
        if (sent == SOCKET_ERROR) {
            handle_error("[ERROR] Error sending message", sock);
            closesocket(sock);
            return 0;
        }
        total_sent += sent;
    }
    return 1;
}
