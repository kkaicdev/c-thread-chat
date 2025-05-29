#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#include "../include/server_net.h"
#include "../include/server_utils.h"

HANDLE mutex;
Client* clients[MAX_CLIENTS] = { NULL };

SOCKET init_server_socket() {
    WSADATA wsaData;
    SOCKET server_socket;
    struct sockaddr_in server_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        print_winsock_error(WSAGetLastError());
        return INVALID_SOCKET;
    }

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        printf("[FATAL]Failed to create mutex!\n");
        WSACleanup();
        return INVALID_SOCKET;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        print_winsock_error(WSAGetLastError());
        return INVALID_SOCKET;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        print_winsock_error(WSAGetLastError());
        closesocket(server_socket);
        return INVALID_SOCKET;
    }

    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        print_winsock_error(WSAGetLastError());
        return INVALID_SOCKET;
    }

    printf("[INFO] Server is online...\n");
    return server_socket;
}

void accept_client_connections(SOCKET server_socket) {
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);

    while (1) {
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            print_winsock_error(WSAGetLastError());
            continue;
        }

        HANDLE pending_thread = CreateThread(NULL, 0, handle_pending_client, (LPVOID)client_socket, 0, NULL);
        if (pending_thread == NULL) {
            printf("[ERROR] Failed to create pending client thread.\n");
            closesocket(client_socket);
        } else {
            CloseHandle(pending_thread);
        }
    }
}

int broadcast_message(const char *message, SOCKET sender_socket) {
    int sent_count = 0;
    WaitForSingleObject(mutex, INFINITE);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && clients[i]->socket != sender_socket) {
            int result = send(clients[i]->socket, message, strlen(message), 0);
            if (result == SOCKET_ERROR) {
                print_winsock_error(WSAGetLastError());
            } else {
                sent_count++;
            }
        }
    }
    ReleaseMutex(mutex);
    return sent_count;
}

DWORD WINAPI handle_pending_client(LPVOID lpParam) {
    SOCKET client_socket = (SOCKET)lpParam;
    char name_buffer[BUFFER_SIZE];

    send_accept_response(client_socket);
    printf("[INFO] Client connected. Waiting for name...\n");

    ClientResult status = receive_client_name(client_socket, name_buffer, sizeof(name_buffer));
    if (status != CLIENT_SUCCESS) {
        closesocket(client_socket);
        return 1;
    }

    WaitForSingleObject(mutex, INFINITE);
    if (is_unique_client_name(name_buffer) == NAME_NOT_UNIQUE) {
        ReleaseMutex(mutex);
        closesocket(client_socket);
        return 1;
    }

    if (add_client(client_socket, name_buffer) != CLIENT_SUCCESS) {
        ReleaseMutex(mutex);
        closesocket(client_socket);
        return 1;
    }
    ReleaseMutex(mutex);

    HANDLE client_thread = CreateThread(NULL, 0, handle_client_thread, (LPVOID)client_socket, 0, NULL);
    if (client_thread == NULL) {
        printf("[ERROR] Failed to create client thread.\n");
        remove_client(client_socket);
        closesocket(client_socket);
    } else {
        CloseHandle(client_thread);
    }

    return 0;
}

DWORD WINAPI handle_client_thread(LPVOID lpParam) {
    SOCKET client_socket = (SOCKET)lpParam;
    char buffer[BUFFER_SIZE];
    Client *client = find_client_by_socket(client_socket);

    if (client == NULL) {
        printf("[ERROR] Client not found.\n");
        closesocket(client_socket);
        return 1;
    }

    while (1) {
        int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (recv_size <= 0) {
            printf("[INFO] Connection closed or error.\n");
            break;
        }

        buffer[recv_size] = '\0';
        printf("[MESSAGE] %s says: %s\n", client->name ? client->name : "(unknown)", buffer);

        char message[BUFFER_SIZE];
        snprintf(message, BUFFER_SIZE, "%s says: %s", client->name, buffer);
        broadcast_message(message, client_socket);
    }

    WaitForSingleObject(mutex, INFINITE);
    remove_client(client_socket);
    ReleaseMutex(mutex);
    return 0;
}

Client* find_client_by_socket(SOCKET socket) {
    WaitForSingleObject(mutex, INFINITE);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && clients[i]->socket == socket) {
            ReleaseMutex(mutex);
            return clients[i];
        }
    }
    ReleaseMutex(mutex);
    return NULL;
}
