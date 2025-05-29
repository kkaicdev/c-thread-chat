#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include <winsock2.h>
#include <windows.h>

#define BUFFER_SIZE 1024

int init_winsock();
SOCKET create_and_connect_socket(struct sockaddr_in *server);
DWORD WINAPI receive_messages(LPVOID sock_ptr);
int send_message(SOCKET sock, const char *message);

#endif