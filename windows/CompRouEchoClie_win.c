#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024
void errorHandling(const char* msg);

int main(int argc, char* argv[]) {
  WSADATA wsaData;
  SOCKET hSocket;
  SOCKADDR_IN servAddr;
  char msg[BUF_SIZE];
  int strLen, readLen;

  if (argc != 3) {
    printf("Usage: %s <IP> <port>\n", argv[0]);
    return -1;
  }
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    errorHandling("WSAStartup() error");
  }

  hSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (hSocket == INVALID_SOCKET) {
    errorHandling("socket() error");
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(argv[1]);
  servAddr.sin_port = htons(atoi(argv[2]));

  if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) ==
      SOCKET_ERROR) {
    errorHandling("connect() error");
  } else {
    puts("connected......");
  }

  while (1) {
    fputs("Input msg(Q to quit): ", stdout);
    fgets(msg, BUF_SIZE, stdin);
    if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
      break;
    }
    strLen = strlen(msg);
    send(hSocket, msg, strLen, 0);
    readLen = 0;
    while (1) {
      readLen += recv(hSocket, &msg[readLen], BUF_SIZE - 1, 0);
      if (readLen >= strLen) {
        break;
      }
    }
    msg[strLen] = 0;
    printf("Message from server: %s\n", msg);
  }

  closesocket(hSocket);
  WSACleanup();
  return 0;
}

void errorHandling(const char* msg) {
  fputs(msg, stderr);
  fputs("\n", stderr);
  exit(-1);
}