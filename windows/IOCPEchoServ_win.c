#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct {  // socket info
  SOCKET clieSock;
  SOCKADDR_IN clieAddr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct {  // buffer info
  OVERLAPPED overlapped;
  WSABUF wsaBuf;
  char buf[BUF_SIZE];
  int mode;  // Read or Write.
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI echoThreadFunc(LPVOID pComPort);
void errorHandling(const char* msg);

int main(int argc, char* argv[]) {
  WSADATA wsaData;
  HANDLE hComPort;
  SYSTEM_INFO sysInfo;
  LPPER_HANDLE_DATA handleInfo;
  LPPER_IO_DATA ioInfo;

  SOCKET hServSock;
  SOCKADDR_IN servAddr;
  int recvBytes, flags = 0;

  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return -1;
  }
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    errorHandling("WSAStartup() error.");
  }

  hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  GetSystemInfo(&sysInfo);
  for (int i = 0; i < sysInfo.dwNumberOfProcessors; ++i) {
    _beginthreadex(NULL, 0, echoThreadFunc, (LPVOID)hComPort, 0, NULL);
  }

  hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(atoi(argv[1]));

  if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
    errorHandling("bind() error.");
  }
  if (listen(hServSock, 5) == SOCKET_ERROR) {
    errorHandling("listen() error.");
  }

  while (1) {
    SOCKET hClieSock;
    SOCKADDR_IN clieAddr;
    int addrLen = sizeof(clieAddr);

    hClieSock = accept(hServSock, (SOCKADDR*)&clieAddr, &addrLen);
    if (hClieSock == INVALID_SOCKET) {
      errorHandling("accept() error.");
    }
    handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
    handleInfo->clieSock = hClieSock;
    memcpy(&(handleInfo->clieAddr), &clieAddr, addrLen);

    CreateIoCompletionPort((HANDLE)hClieSock, hComPort, (DWORD)handleInfo, 0);

    ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
    ioInfo->wsaBuf.buf = ioInfo->buf;
    ioInfo->wsaBuf.len = BUF_SIZE;
    ioInfo->mode = READ;
    WSARecv(handleInfo->clieSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
  }
  return 0;
}

DWORD WINAPI echoThreadFunc(LPVOID pComPort) {
  HANDLE hComPort = (HANDLE)pComPort;
  SOCKET sock;
  DWORD bytesTrans;
  LPPER_HANDLE_DATA handleInfo;
  LPPER_IO_DATA ioInfo;
  DWORD flags = 0;
  
  while (1) {
    GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED)&ioInfo, INFINITE);
    sock = handleInfo->clieSock;

    if (ioInfo->mode == READ) {
      puts("message received.");
      if (bytesTrans == 0) {
        closesocket(sock);
        free(handleInfo);
        free(ioInfo);
        continue;
      }

      memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
      ioInfo->wsaBuf.len = bytesTrans;
      ioInfo->mode = WRITE;
      WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
      ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
      memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
      ioInfo->wsaBuf.buf = ioInfo->buf;
      ioInfo->wsaBuf.len = BUF_SIZE;
      ioInfo->mode = READ;
      WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
    } else {
      puts("message sent.");
      free(ioInfo);
    }
  }
  return 0;
}

void errorHandling(const char* msg) {
  fputs(msg, stderr);
  fputs("\n", stderr);
  exit(-1);
}