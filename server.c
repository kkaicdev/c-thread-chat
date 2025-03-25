#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 3

typedef struct {
    SOCKET socket;
    HANDLE thread_handle;
    int id;
    char name[50];
} Client;

HANDLE mutex;
Client *clients[MAX_CLIENTS];
int client_count = 0;

void print_winsock_error(int error_code);
DWORD WINAPI handle_client(LPVOID lpParam);
int is_name_unique(const char *name);
void broadcast_message(const char *message, SOCKET sender_socket);
void remove_client(int client_index);
void lock_client_list();
void unlock_client_list();

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (!mutex) { 
        printf("Failed to create mutex\n"); 
        return -1; 
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { 
        print_winsock_error(WSAGetLastError()); 
        return -1; 
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) { 
        print_winsock_error(WSAGetLastError()); 
        return -1; 
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) { 
        print_winsock_error(WSAGetLastError()); 
        return -1; 
    }

    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) { 
        print_winsock_error(WSAGetLastError()); 
        return -1; 
    }

    printf("Server is online...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            print_winsock_error(WSAGetLastError());
            continue;
        }

        if (client_count >= MAX_CLIENTS) {
            const char *response = "NOT";
            send(client_socket, response, strlen(response) + 1, 0);
            closesocket(client_socket);
            continue;
        }

        const char *response = "OK";
        send(client_socket, response, strlen(response) + 1, 0);

        printf("Client connected. Waiting for name...\n");

        char client_name[50];
        int recv_size = recv(client_socket, client_name, sizeof(client_name) - 1, 0);
        if (recv_size > 0) {
            client_name[recv_size] = '\0';
            if (is_name_unique(client_name)) {
                lock_client_list();
                Client *new_client = malloc(sizeof(Client));
                if (!new_client) {
                    printf("Memory allocation failed.\n");
                    closesocket(client_socket);
                    unlock_client_list();
                    continue;
                }
                strncpy(new_client->name, client_name, sizeof(new_client->name) - 1);
                new_client->socket = client_socket;
                new_client->id = client_count + 1;
                clients[client_count++] = new_client;
                unlock_client_list();
                
                printf("Client's name: %s\n", client_name);
                char welcome_message[BUFFER_SIZE];
                snprintf(welcome_message, BUFFER_SIZE, "Client %s has joined!\n", client_name);
                broadcast_message(welcome_message, client_socket);

                new_client->thread_handle = CreateThread(NULL, 0, handle_client, (LPVOID)new_client, 0, NULL);
                if (new_client->thread_handle == NULL) {
                    printf("Failed to create thread for client %s.\n", client_name);
                    closesocket(client_socket);
                    remove_client(client_count - 1);
                }
            } else {
                printf("Name in use. Disconnecting...\n");
                closesocket(client_socket);
            }
        } else {
            print_winsock_error(WSAGetLastError());
            closesocket(client_socket);
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

DWORD WINAPI handle_client(LPVOID lpParam) {
    Client *client = (Client *)lpParam;
    SOCKET client_socket = client->socket;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (read_size <= 0) {
            printf("Client %s disconnected.\n", client->name);
            char disconnect_message[BUFFER_SIZE];
            snprintf(disconnect_message, BUFFER_SIZE, "%s has left the chat.\n", client->name);
            broadcast_message(disconnect_message, client_socket);
            break;
        } else {
            buffer[read_size] = '\0';
            printf("Client %s: %s\n", client->name, buffer);
            char message[BUFFER_SIZE];
            snprintf(message, BUFFER_SIZE, "%s says: %s", client->name, buffer);
            broadcast_message(message, client_socket);
        }
    }

    remove_client(client->id - 1);  
    free(client);
    return 0;
}

int is_name_unique(const char* name) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i]->name, name) == 0) {
            return 0;
        }
    }
    return 1;
}

void broadcast_message(const char *message, SOCKET sender_socket) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i]->socket != sender_socket) {
            send(clients[i]->socket, message, strlen(message) + 1, 0);
        }
    }
}

void remove_client(int client_index) {
    if (client_index >= 0 && client_index < client_count) {
        lock_client_list();
        closesocket(clients[client_index]->socket);
        free(clients[client_index]);
        
        memmove(&clients[client_index], &clients[client_index + 1], (client_count - client_index - 1) * sizeof(Client*));

        clients[client_count - 1] = NULL;
        client_count--;
        unlock_client_list();
    }
}


void lock_client_list() { 
    WaitForSingleObject(mutex, INFINITE); 
}

void unlock_client_list() { 
    ReleaseMutex(mutex); 
}

void print_winsock_error(int error_code) {
    char *msg = NULL;
    DWORD dwSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, (LPSTR)&msg, 0, NULL);
    if (dwSize) {
        printf("WinSock error: %s\n", msg);
        LocalFree(msg);
    } else {
        printf("Unexpected error occurred.\n");
    }
}
