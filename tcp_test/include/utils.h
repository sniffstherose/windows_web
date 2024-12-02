#pragma once
#include <winsock2.h>

class SockInfo {
 public:
  SOCKET hSocket;
  SOCKADDR_IN* haddr;
};

class IoInfo {
 public:
  OVERLAPPED overlapped;
  WSABUF wsaBuf;
  bool mode;
};