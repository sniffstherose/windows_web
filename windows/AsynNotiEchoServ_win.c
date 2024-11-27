#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 100

void compressSockets(SOCKET hSockArr[], int index, int total);
void compressEvents(WSAEVENT hEventArr[], int index, int total);
void errorHandling(const char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClieSock;
    SOCKADDR_IN servAddr, clieAddr;

    SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT newEvent;
    WSANETWORKEVENTS netEvents;

    int numOfClieSock = 0;
    int strLen;
    int posInfo, startIdx;
    int clieLen;
    char msg[BUF_SIZE];

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        errorHandling("WSAStartup() error!");
    }

    hServSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        errorHandling("bind() error!");
    }
    if (listen(hServSock, 5) == SOCKET_ERROR) {
        errorHandling("listen() error!");
    }

    newEvent = WSACreateEvent();
    if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR) {
        errorHandling("WSAEventSelect() error!");
    }

    hSockArr[numOfClieSock] = hServSock;
    hEventArr[numOfClieSock] = newEvent;
    ++numOfClieSock;

    while(1) {
        posInfo = WSAWaitForMultipleEvents(numOfClieSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
        startIdx = posInfo - WSA_WAIT_EVENT_0;

        for (int i = startIdx; i < numOfClieSock; ++i) {
            int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
            if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT) {
                continue;
            } else {
                sigEventIdx = i;
                WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);
                if (netEvents.lNetworkEvents & FD_ACCEPT) { // 新连接
                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
                        puts("Accept error!");
                        break;
                    }
                    clieLen = sizeof(clieAddr);
                    hClieSock = accept(hSockArr[numOfClieSock], (SOCKADDR*)&clieAddr, &clieLen);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(hClieSock, newEvent, FD_READ | FD_CLOSE);

                    hSockArr[numOfClieSock] = hClieSock;
                    hEventArr[numOfClieSock] = newEvent;
                    ++numOfClieSock;
                    puts("connected new client...");
                }
                if (netEvents.lNetworkEvents & FD_READ) {   // 有数据可读
                    if (netEvents.iErrorCode[FD_READ_BIT] != 0) {
                        puts("read error!");
                        break;
                    }
                    strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
                    send(hSockArr[sigEventIdx], msg, strLen, 0);
                }
                if (netEvents.lNetworkEvents & FD_CLOSE) {  // 关闭连接
                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
                        puts("close error!");
                        break;
                    }
                    WSACloseEvent(hEventArr[sigEventIdx]);
                    closesocket(hSockArr[sigEventIdx]);

                    --numOfClieSock;
                    compressSockets(hSockArr, sigEventIdx, numOfClieSock);
                    compressEvents(hEventArr, sigEventIdx, numOfClieSock);
                }
            }
        }
    }
    WSACleanup();
    closesocket(hServSock);
    return 0;
}

void compressSockets(SOCKET hSockArr[], int index, int total) {
    for (int i = index; i < total; ++i) {
        hSockArr[i] = hSockArr[i + 1];
    }
}

void compressEvents(WSAEVENT hEventArr[], int index, int total) {
    for (int i = index; i < total; ++i) {
        hEventArr[i] = hEventArr[i + 1];
    }
}

void errorHandling(const char* msg) {
    fputs(msg, stdout);
    fputs("\n", stdout);
    exit(1);
}