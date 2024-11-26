#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

void errorHandling(char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServerSock, hClientSock;
    SOCKADDR_IN serverAddr, clientAddr;
    int clientAddrSize;
    char message[] = "hello world";
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStartUp() error!");
    }

    hServerSock = socket(AF_INET, SOCK_STREAM, 0);
    if (hServerSock == INVALID_SOCKET) {
        errorHandling("SOCK() error!");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServerSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        errorHandling("bind() error!");
    }

    if (listen(hServerSock, 5) == SOCKET_ERROR) {
        errorHandling("listen() error!");
    }

    clientAddrSize = sizeof(clientAddr);
    hClientSock = accept(hServerSock, (SOCKADDR*)&clientAddr, &clientAddrSize);
    if (hClientSock == INVALID_SOCKET) {
        errorHandling("accept() error!");
    }

    send(hClientSock, message, sizeof(message), 0);
    closesocket(hClientSock);
    closesocket(hServerSock);
    WSACleanup();
    return 0;
}

void errorHandling(char* message) {
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}