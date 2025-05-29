#ifndef UTILS_H
#define UTILS_H

#include <winsock2.h>

void print_winsock_error(int error_code);
void send_accept_response(SOCKET socket);
void send_reject_response(SOCKET socket);

#endif