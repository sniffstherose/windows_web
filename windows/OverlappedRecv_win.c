#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

void errorHandling(const char* arg);
void CALLBACK compRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

WSABUF dataBuf;
char buf[BUF_SIZE];
int recvBytes = 0;

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hSocket, hRecvSock;
    SOCKADDR_IN hSendAddr, hRecvAddr;
    int recvLen;

    WSAEVENT hEvent;
    WSAOVERLAPPED overlapped;

    int idx, recvAddrSz, flags = 0;
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStarup() error!");
    }

    hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&hRecvAddr, 0, sizeof(hRecvAddr));
    hRecvAddr.sin_family = AF_INET;
    hRecvAddr.sin_addr.s_addr = INADDR_ANY;
    hRecvAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hSocket, (SOCKADDR*)&hRecvAddr, sizeof(hRecvAddr)) == SOCKET_ERROR) {
        errorHandling("bind() error!");
    }
    if (listen(hSocket, 5) == SOCKET_ERROR) {
        errorHandling("listen() error!");
    }

    recvLen = sizeof(hSendAddr);
    hRecvSock = accept(hSocket, (SOCKADDR*)&hSendAddr, &recvLen);

    hEvent = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = hEvent;
    dataBuf.len = BUF_SIZE;
    dataBuf.buf = buf;
    if (WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, compRoutine) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            puts("Background data receive");
        }
    }

    idx = WSAWaitForMultipleEvents(1, &hEvent, FALSE, WSA_INFINITE, TRUE);
    if (idx == WAIT_IO_COMPLETION) {
        puts("Overlapped I/O completed.");
    } else {
        errorHandling("WSARecv() error!");
    }

    WSACloseEvent(hEvent);
    closesocket(hSocket);
    closesocket(hRecvSock);
    WSACleanup();
    return 0;
}

void errorHandling(const char* msg) {
    fputs(msg, stderr);
    fputs("\n", stderr);
    exit(-1);
}

void CALLBACK compRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    if (dwError != 0) {
        errorHandling("compRoutine() error!");
    } else {
        recvBytes = szRecvBytes;
        printf("Received message: %s\n", buf);
    }
}