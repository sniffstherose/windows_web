#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

void errorHandling(char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET connSock;
    SOCKADDR_IN serverAddr;

    char message[30] = {0};
    int strLen;
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStartUp() error!");
    }

    connSock = socket(AF_INET, SOCK_STREAM, 0);
    if (connSock == INVALID_SOCKET) {
        errorHandling("socket() error!");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));

    if (connect(connSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        errorHandling("connect() error");
    }

    strLen = recv(connSock, message, sizeof(message) - 1, 0);
    if (strLen == -1) {
        errorHandling("read() error!");
    }
    printf("Message from server: %s\n", message);

    closesocket(connSock);
    WSACleanup();
    return 0;
}

void errorHandling(char* message) {
    fputs(message, stdout);
    fputs("\n", stdout);
    exit(1);
}