#include <winsock2.h>
#include <stdio.h>
#include "../include/server_net.h"

int main() {
    SOCKET server_socket = init_server_socket();
    if (server_socket == INVALID_SOCKET) {
        printf("[FATAL] Failed to initialize server socket!\n");
        WSACleanup();
        return 1;
    }

    printf("[INFO] Starting to accept client connections...\n");
    accept_client_connections(server_socket);

    printf("[INFO] Shutting down server.\n");
    closesocket(server_socket);
    WSACleanup();
    return 0;
}