#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h>

#define PORT 8080
#define SERVER_IP "" // <--- IP SERVER
#define BUFFER_SIZE 1024
#define MAX_NAME_LENGTH 49

int init_winsock();
SOCKET create_and_connect_socket(struct sockaddr_in *server);
int get_client_name(char *client_name);
int send_message(SOCKET sock, const char *message);
void handle_error(const char *msg, SOCKET sock);
unsigned __stdcall receive_messages(void *sock_ptr);

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
    printf("Connecting to the server...\n");

    char server_response[4];
    if (recv(sock, server_response, sizeof(server_response), 0) == SOCKET_ERROR) {
        handle_error("Error receiving response from the server", sock);
    }

    if (strncmp(server_response, "OK", 2) == 0) {
        printf("Connection successful.\n");
        if (!get_client_name(client_name)) return 1;
        send_message(sock, client_name);
        printf("You can now send messages. Type 'exit' to disconnect.\n");
    } else {
        printf("Connection failed.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    unsigned int thread_id;
    HANDLE thread_handle = (HANDLE)_beginthreadex(NULL, 0, receive_messages, (void *)&sock, 0, &thread_id);
    if (thread_handle == NULL) handle_error("Error creating thread to receive messages", sock);

    while (1) {
        printf("Enter your message: ");
        fgets(message, sizeof(message), stdin);
        
        if (strncmp(message, "sair", 4) == 0 || strncmp(message, "disconnect", 10) == 0) {
            send_message(sock, "disconnect");
            break;
        }
        send_message(sock, message);
    }

    printf("Disconnected.\n");
    closesocket(sock);
    WSACleanup();
    return 0;
}

void handle_error(const char *msg, SOCKET sock) {
    printf("%s: %d\n", msg, WSAGetLastError());
    if (sock != INVALID_SOCKET) closesocket(sock);
    WSACleanup();
    exit(1);
}

unsigned __stdcall receive_messages(void *sock_ptr) {
    SOCKET sock = *(SOCKET *)sock_ptr;
    char buffer[BUFFER_SIZE];

    while (1) {
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            if (n == 0) printf("Connection closed by the server.\n");
            else handle_error("Error receiving message", sock);
            break;
        }
        buffer[n] = '\0';
        printf("\nMessage received:\n%s\n", buffer);
        printf("Enter your message: ");
    }
    return 0;
}

int init_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

SOCKET create_and_connect_socket(struct sockaddr_in *server) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) handle_error("Error creating socket", sock);
    if (connect(sock, (struct sockaddr *)server, sizeof(*server)) == SOCKET_ERROR) handle_error("Erro ao conectar", sock);
    return sock;
}

int get_client_name(char *client_name) {
    printf("Enter your name: ");
    fgets(client_name, MAX_NAME_LENGTH + 1, stdin);
    client_name[strcspn(client_name, "\n")] = 0;

    if (strlen(client_name) == 0) {
        printf("Name cannot be empty.\n");
        return 0;
    }
    if (strlen(client_name) > MAX_NAME_LENGTH) {
        printf("Name too long. Maximum %d characters.\n", MAX_NAME_LENGTH);
        return 0;
    }
    return 1;
}

int send_message(SOCKET sock, const char *message) {
    if (send(sock, message, strlen(message) + 1, 0) == SOCKET_ERROR) {
        handle_error("Error sending message", sock);
        return 0;
    }
    return 1;
}
