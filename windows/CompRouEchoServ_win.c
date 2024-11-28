#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024
void CALLBACK readCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK writeCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void errorHandling(const char* msg);

typedef struct {
  SOCKET hClieSock;
  char buf[BUF_SIZE];
  WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char* argv[]) {
  WSADATA wsaData;
  SOCKET hLisnSock, hRecvSock;
  SOCKADDR_IN lisnAddr, recvAddr;
  LPWSAOVERLAPPED lpOvLp;
  DWORD recvBytes;
  LPPER_IO_DATA hbInfo;
  int mode = 1, recvAdrSz, flagInfo = 0;

  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return -1;
  }
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    errorHandling("WSAStartup() error!");
  }

  hLisnSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  ioctlsocket(hLisnSock, FIONBIO, &mode);

  memset(&lisnAddr, 0, sizeof(lisnAddr));
  lisnAddr.sin_family = AF_INET;
  lisnAddr.sin_addr.s_addr = INADDR_ANY;
  lisnAddr.sin_port = htons(atoi(argv[1]));

  if (bind(hLisnSock, (SOCKADDR*)&lisnAddr, sizeof(lisnAddr)) == SOCKET_ERROR) {
    errorHandling("bind() error");
  }
  if (listen(hLisnSock, 5) == SOCKET_ERROR) {
    errorHandling("listen() error");
  }

  recvAdrSz = sizeof(recvAddr);
  while (1) {
    SleepEx(100, TRUE);
    hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAddr, &recvAdrSz);
    if (hRecvSock == INVALID_SOCKET) {
      if (WSAGetLastError() == WSAEWOULDBLOCK) {
        continue;
      } else {
        errorHandling("accept() error");
      }
    }
    puts("Client connected.");

    lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
    memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

    hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    hbInfo->hClieSock = (DWORD)hRecvSock;
    (hbInfo->wsaBuf).buf = hbInfo->buf;
    (hbInfo->wsaBuf).len = BUF_SIZE;

    lpOvLp->hEvent = (HANDLE)hbInfo;
    WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp,
            readCompRoutine);
  }
  closesocket(hRecvSock);
  closesocket(hLisnSock);
  WSACleanup();
  return 0;
}

void CALLBACK readCompRoutine(DWORD dwError, DWORD szRecvBytes,
                              LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
  LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
  SOCKET hSock = hbInfo->hClieSock;
  LPWSABUF bufInfo = &(hbInfo->wsaBuf);
  DWORD sentBytes;

  if (szRecvBytes == 0) {
    closesocket(hSock);
    free(lpOverlapped->hEvent);
    free(lpOverlapped);
    puts("Client disconnected.");
  } else {
    bufInfo->len = szRecvBytes;
    WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, writeCompRoutine);
  }
}

void CALLBACK writeCompRoutine(DWORD dwError, DWORD szSendBytes,
                               LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
  LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
  SOCKET hSock = hbInfo->hClieSock;
  LPWSABUF bufInfo = &(hbInfo->wsaBuf);
  DWORD recvBytes;
  int flagInfo = 0;
  WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped,
          readCompRoutine);
}

void errorHandling(const char* msg) {
  fputs(msg, stderr);
  fputs("\n", stderr);
  exit(-1);
}