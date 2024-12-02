#include "terminal.h"
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

class TcpTestClient : public Terminal {
 public:
  TcpTestClient(int cnt, std::string ip, int port);
  // ~TcpTestClient();
  void run() override;

 private:
  // void worker_thread_func() override;
  // void async_send() override;
  // void async_recv() override;
 private:
  std::string server_ip_;
  int server_port_;
  int connection_count_;
  SOCKADDR_IN server_addr_;
  SOCKADDR_IN client_addr_;
};