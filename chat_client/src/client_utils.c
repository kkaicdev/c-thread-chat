#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include "../include/client_utils.h"

void handle_error(const char *msg, SOCKET sock) {
    printf("%s: %d\n", msg, WSAGetLastError());

    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }

    WSACleanup();
    exit(1);
}

int get_client_name(char *client_name) {
    printf("Enter your name: ");
    fgets(client_name, MAX_NAME_LENGTH + 1, stdin);

    if (strchr(client_name, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    client_name[strcspn(client_name, "\n")] = 0;

    if (strlen(client_name) == 0) {
        printf("Please enter a name (1 to %d characters).\n", MAX_NAME_LENGTH);
        return 0;
    }

    return 1;
}
