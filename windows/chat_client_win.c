#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI sendMsg(void* arg);
unsigned WINAPI recvMsg(void* arg);
void errorHandling(const char* msg);

char name[NAME_SIZE] = "[default]";
char msg[BUF_SIZE];

int main(int argc, char* argv[])  {
    WSADATA wsaData;
    SOCKET serverSock;
    SOCKADDR_IN serverAddr;
    HANDLE hSendThread, hRecvThread;
    if (argc != 4) {
        printf("Usage: %s <IP> <port> <name>\n", argv[0]);
        return -1;
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStartup() error!");
    }

    sprintf(name, "[%s]", argv[3]);
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));

    if (connect(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        errorHandling("connect() error!");
    }

    hSendThread = (HANDLE)_beginthreadex(NULL, 0, sendMsg, (void*)&serverSock, 0, NULL);
    hRecvThread = (HANDLE)_beginthreadex(NULL, 0, recvMsg, (void*)&serverSock, 0, NULL);

    WaitForSingleObject(hSendThread, INFINITE);
    WaitForSingleObject(hRecvThread, INFINITE);

    closesocket(serverSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI sendMsg(void* arg) {
    SOCKET serverSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    while (1) {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            closesocket(serverSock);
            exit(0);
        }
        sprintf(nameMsg, "%s %s", name, msg);
        send(serverSock, nameMsg, strlen(nameMsg), 0);
    }
    return 0;
}

unsigned WINAPI recvMsg(void* arg) {
    SOCKET serverSock = *((SOCKET*)arg);
    char nameMsg[NAME_SIZE + BUF_SIZE];
    int strLen;
    while (1) {
        strLen = recv(serverSock, nameMsg, sizeof(nameMsg), 0);
        if (strLen == -1) {
            errorHandling("recvMsg() error!");
        }
        nameMsg[strLen] = 0;
        fputs(nameMsg, stdout);
    }
    return 0;
}

void errorHandling(const char* msg) {
    fputs(msg, stdout);
    fputs("\n", stdout);
    exit(1);
}
