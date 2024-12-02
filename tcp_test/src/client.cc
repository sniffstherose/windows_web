#include "client.h"

#define READ true
#define WRITE false

TcpTestClient::TcpTestClient(int cnt, std::string ip, int port)
    : connection_count_(cnt), server_ip_(ip), server_port_(port) {
  send_msg_ = "ping\0";
  receive_msg_ = "pong\0";
  completion_port_ =
      CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = inet_addr(server_ip_.c_str());
  server_addr_.sin_port = htons(server_port_);
}

/*
主线程：
1. 创建连接
2. 每隔1秒发起与连接数等同个数的IO操作
*/
void TcpTestClient::run() {
  // 创建 Socket
  SOCKET hSock =
      WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
  if (connect(hSock, (SOCKADDR*)&server_addr_, sizeof(server_addr_)) ==
      SOCKET_ERROR) {
    std::cerr << "connect() error!" << GetLastError() << std::endl;
    exit(-1);
  }

  // 初始化 IoInfo 和 SockInfo
  auto sockInfo = std::make_shared<SockInfo>();
  auto sendIoInfo = std::make_shared<IoInfo>();
  auto recvIoInfo = std::make_shared<IoInfo>();

  sockInfo->hSocket = hSock;
  sockInfo->haddr = nullptr;

  // 设置发送操作
  sendIoInfo->wsaBuf.buf = send_msg_;
  sendIoInfo->wsaBuf.len = 5;
  sendIoInfo->mode = WRITE;
  ZeroMemory(&(sendIoInfo->overlapped), sizeof(OVERLAPPED));

  // 设置接收操作
  char recvBuf[1024]{};
  recvIoInfo->wsaBuf.buf = recvBuf;
  recvIoInfo->wsaBuf.len = 1024;
  recvIoInfo->mode = READ;
  ZeroMemory(&(recvIoInfo->overlapped), sizeof(OVERLAPPED));

  // 将 Socket 和对应的 SockInfo 绑定到 IO 完成端口
  CreateIoCompletionPort((HANDLE)sockInfo->hSocket, completion_port_,
                         (ULONG_PTR)sockInfo.get(), 0);

  // 发起发送和接收操作
  async_send(sockInfo, sendIoInfo);

  // 等待 I/O 操作完成
  while (true) {
    DWORD bytesTrans;
    ULONG_PTR completionKey;
    LPOVERLAPPED overlapped;

    BOOL result = GetQueuedCompletionStatus(
        completion_port_, &bytesTrans, &completionKey, &overlapped, INFINITE);
    if (!result) {
      std::cerr << "I/O operation failed. Error: " << GetLastError()
                << std::endl;
    }

    // 获取到 completionKey 后，它是一个指向 SockInfo 对象的裸指针
    auto* socketInfo = reinterpret_cast<SockInfo*>(completionKey);
    auto* ioInfo = reinterpret_cast<IoInfo*>(overlapped);

    if (ioInfo->mode == READ) {
      std::cout << "Received: " << (ioInfo->wsaBuf).buf << std::endl;
      async_send(sockInfo, sendIoInfo);
    } else {
      std::cout << "ping sent." << std::endl;
      async_recv(sockInfo, recvIoInfo);
    }
  }
}
