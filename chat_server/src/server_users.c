#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/server_net.h"
#include "../include/server_users.h"
#include "../include/server_utils.h"

int client_count = 0;

ClientResult add_client(SOCKET client_socket, const char *name) {

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            Client *new_client = calloc(1, sizeof(Client));
            if (!new_client) {
                return CLIENT_ERROR_ALOC;
            }
            
            new_client->socket = client_socket;
            new_client->id = i + 1;
            strncpy(new_client->name, name, sizeof(new_client->name) -1 );
            new_client->name[sizeof(new_client->name) - 1] = '\0';

            clients[i] = new_client;
            client_count++;
            return CLIENT_SUCCESS;
        }
    }
    printf("[WARNING] Client list is full. Cannot accept new connection.\n");
    return CLIENT_ERROR_LIST_FULL;
}

void remove_client(SOCKET client_socket) {
    WaitForSingleObject(mutex, INFINITE);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && clients[i]->socket == client_socket) {
            printf("[INFO] Client '%s' disconnected.\n", clients[i]->name);

            if (clients[i]->socket != INVALID_SOCKET) {
                int result = closesocket(clients[i]->socket);
                if (result == SOCKET_ERROR) {
                    print_winsock_error(WSAGetLastError());
                }
            }
            free(clients[i]);
            clients[i] = NULL;
            if (client_count > 0) {
                client_count--;
            }
            break;
        }
    }
    ReleaseMutex(mutex);
}

ClientResult receive_client_name(SOCKET client_socket, char *buffer, size_t buffer_len) {
    int recv_size = recv(client_socket, buffer, (int)buffer_len -1, 0);
    if (recv_size <= 0) {
        print_winsock_error(WSAGetLastError());
        return CLIENT_ERROR_RECV;
    }

    buffer[recv_size] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (strlen(buffer) >= MAX_NAME_LEN || strlen(buffer) == 0 || strspn(buffer, " \t") == strlen(buffer)) {
        printf("[ERROR] Client sent an invalid name.\n");
        return CLIENT_ERROR_INVALID_NAME;
    }

    printf("[INFO] Name received: '%s'\n", buffer);
    return CLIENT_SUCCESS;
}

ClientNameCheck is_unique_client_name(const char *name) {
    if (name == NULL) {
        return NAME_NULL;
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL && _stricmp(clients[i]->name, name) == 0) {
            return NAME_NOT_UNIQUE; 
        }
    }
    return NAME_UNIQUE; 
}