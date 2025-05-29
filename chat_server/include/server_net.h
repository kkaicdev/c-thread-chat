#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>

#include "../include/server_users.h"

#define PORT 8080
#define BUFFER_SIZE 1024

extern HANDLE mutex;

SOCKET init_server_socket(void);
void accept_client_connections(SOCKET server_socket);

int broadcast_message(const char *message, SOCKET sender_socket);

DWORD WINAPI handle_client_thread(LPVOID lpParam);
DWORD WINAPI handle_pending_client(LPVOID lpParam);
Client* find_client_by_socket(SOCKET socket);

#endif