#include <stdio.h>
#include <winsock2.h>
#include "client_net.h"
#include "client_utils.h"

#define SERVER_IP "192.168.1.11" // <--- IP SERVER
#define PORT 8080                // <--- PORT
#define BUFFER_SIZE 1024

int main() {
    SOCKET sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    char client_name[MAX_NAME_LENGTH + 1];

    if (!init_winsock()) return 1;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    sock = create_and_connect_socket(&server);
    
    char server_response[4];
    if (recv(sock, server_response, sizeof(server_response), 0) == SOCKET_ERROR) {
        handle_error("[ERROR] Error receiving response from the server.\n", sock);
        closesocket(sock);
        return 1;
    }

    if (strcmp(server_response, "OK") == 0) {

        if (!get_client_name(client_name)) {
            closesocket(sock);
            return 1;
        }
        
        send_message(sock, client_name);
        printf("[INFO] You can now send messages. Type 'exit' or 'disconnect' to close the connection.\n");

    } else {
        printf("[ERROR] Connection rejected: probably server full.\n");
        closesocket(sock);
        return 1;
    }

    HANDLE thread_handle = CreateThread(NULL, 0, receive_messages, (LPVOID)&sock, 0, NULL);
    if (thread_handle == NULL) {
        handle_error("[ERROR] Error creating thread to receive messages.\n", sock);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (1) {
        printf("Enter your message: ");
        fflush(stdout);
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0 || strcmp(message, "disconnect") == 0) {
            break;
        }

        if (!send_message(sock, message)) {
            printf("[ERROR] Failed to send message. Disconnecting...\n");
            break;
        }
    }

    printf("Disconnected.\n");
    closesocket(sock);
    WSACleanup();
    CloseHandle(thread_handle);

    return 0;
}
