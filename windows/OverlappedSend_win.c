#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void errorHandling(const char* arg);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN hSendAddr;

    WSABUF dataBuf;
    char msg[] = "Computer network.";
    int sentBytes = 0;

    WSAEVENT hEvent;
    WSAOVERLAPPED overlapped;

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        return -1;
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStarup() error!");
    }

    hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&hSendAddr, 0, sizeof(hSendAddr));
    hSendAddr.sin_family = AF_INET;
    hSendAddr.sin_addr.s_addr = inet_addr(argv[1]);
    hSendAddr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR*)&hSendAddr, sizeof(hSendAddr)) == SOCKET_ERROR) {
        errorHandling("connect() error!");
    }

    hEvent = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = hEvent;
    dataBuf.len = strlen(msg) + 1;
    dataBuf.buf = msg;
    if (WSASend(hSocket, &dataBuf, 1, &sentBytes, 0, &overlapped, NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            puts("Background data sending.");
            WSAWaitForMultipleEvents(1, &hEvent, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hSocket, &overlapped, &sentBytes, FALSE, NULL);
        } else {
            errorHandling("WSASend() error!");
        }
    }

    Sleep(1000);
    printf("sent data size: %d\n", sentBytes);
    WSACloseEvent(hEvent);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void errorHandling(const char* msg) {
    fputs(msg, stderr);
    fputs("\n", stderr);
    exit(-1);
}