#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLIENTS 256

unsigned WINAPI handleClient(void* arg);
void sendMsg(char* msg, int len);
void errorHandling(const char* msg);

int clientCnt = 0;
SOCKET clients[MAX_CLIENTS];
HANDLE hMutex;

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServerSock, hClientSock;
    SOCKADDR_IN serverAddr,  clientAddr;
    int clientAddrSize;
    HANDLE hThread;
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStartup() error!");
    }

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hServerSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServerSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        errorHandling("bind() error!");
    }
    if (listen(hServerSock, 5) == SOCKET_ERROR) {
        errorHandling("listen() error!");
    }

    while (1) {
        clientAddrSize = sizeof(clientAddr);
        hClientSock = accept(hServerSock, (SOCKADDR*)&clientAddrSize, &clientAddrSize);

        WaitForSingleObject(hMutex, INFINITE);
        clients[clientCnt++] = hClientSock;
        ReleaseMutex(hMutex);

        hThread = (HANDLE)_beginthreadex(NULL, 0, handleClient, (void*)&hClientSock, 0, NULL);
        printf("connected client ip: %s\n", inet_ntoa(clientAddr.sin_addr));
    }

    closesocket(hServerSock);
    CloseHandle(hMutex);
    WSACleanup();
    return 0;
}

unsigned WINAPI handleClient(void* arg) {
    SOCKET hClientSock = *((SOCKET*)arg);
    int strLen = 0;
    char msg[BUF_SIZE];

    while (strLen = recv(hClientSock, msg, sizeof(msg), 0) != 0) {
        sendMsg(msg, strLen);
    }

    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < clientCnt; ++i) {
        if (hClientSock == clients[i]) {
            while (i++ < clientCnt - 1) {
                clients[i] = clients[i + 1];
            }
            break;
        }
    }
    --clientCnt;
    ReleaseMutex(hMutex);
    closesocket(hClientSock);
    return 0;
}

void sendMsg(char* msg, int len) {
    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < clientCnt; ++i) {
        send(clients[i], msg, len, 0);
    }
    ReleaseMutex(hMutex);
}

void errorHandling(const char* msg) {
    fputs(msg, stdout);
    fputs("\n", stdout);
    exit(1);
}