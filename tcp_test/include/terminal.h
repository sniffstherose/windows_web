#ifndef TERMINAL_H_
#define TERMINAL_H_
#include "utils.h"
#include <winsock2.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <string>
#include <memory>

class Terminal {
 public:
  Terminal() {
    threads_number_ = std::thread::hardware_concurrency();
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
      std::cerr << "WSAStartup() error!" << std::endl;
      exit(-1);
    }
  }
  virtual void run() = 0;
  virtual ~Terminal() { WSACleanup(); }

 protected:
  // virtual void worker_thread_func() = 0;
  // virtual void async_send() = 0;
  // virtual void async_recv() = 0;
  void async_send(std::shared_ptr<SockInfo> sockInfo,
                  std::shared_ptr<IoInfo> ioInfo) {
    // 发起发送操作
    WSASend(sockInfo->hSocket, &(ioInfo->wsaBuf), 1, nullptr, 0,
            &(ioInfo->overlapped), nullptr);
  }

  void async_recv(std::shared_ptr<SockInfo> sockInfo,
                  std::shared_ptr<IoInfo> ioInfo) {
    // 发起接收操作
    WSARecv(sockInfo->hSocket, &(ioInfo->wsaBuf), 1, nullptr, 0,
            &(ioInfo->overlapped), nullptr);
  }

 protected:
  WSADATA wsaData_;
  HANDLE completion_port_;
  int threads_number_;  // 并发线程数
  char* send_msg_;      // 准备发送的消息
  char* receive_msg_;   // 期待接收的消息
};

#endif