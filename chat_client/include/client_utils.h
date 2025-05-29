#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <winsock2.h>
#include <windows.h>

#define MAX_NAME_LENGTH 49

int get_client_name(char *client_name);
void handle_error(const char *msg, SOCKET sock);


#endif
