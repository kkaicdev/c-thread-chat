#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <winsock2.h>

#define MAX_CLIENTS 3
#define MAX_NAME_LEN 50

typedef struct Client {
    SOCKET socket;
    int id;
    char name[MAX_NAME_LEN];
} Client;

typedef enum {
    CLIENT_SUCCESS = 0,
    CLIENT_ERROR_ALOC = -1,
    CLIENT_ERROR_NAME_EXISTS = -2,
    CLIENT_ERROR_INVALID_NAME = -3,
    CLIENT_ERROR_LIST_FULL = -4,
    CLIENT_ERROR_RECV = -5,
} ClientResult;

typedef enum {
    NAME_NOT_UNIQUE = 0,
    NAME_NULL = 1,
    NAME_UNIQUE = 2,
} ClientNameCheck;

extern int client_count;
extern Client* clients[MAX_CLIENTS];

ClientResult add_client(SOCKET client_socket, const char *name);
void remove_client(SOCKET client_socket);
ClientResult receive_client_name(SOCKET client_socket, char *buffer, size_t buffer_len);
ClientNameCheck is_unique_client_name(const char *name);

#endif