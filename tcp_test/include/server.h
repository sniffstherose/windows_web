#include "terminal.h"
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>

class TcpTestServer : public Terminal {
 public:
  TcpTestServer(int port);
  //   ~TcpTestServer();
  void run() override;

 private:
  // void worker_thread_func() override;
  // void async_send() override;
  // void async_recv() override;
 private:
  int server_port_;
  int connection_count_;
  SOCKADDR_IN server_addr_;
  SOCKADDR_IN client_addr_;
  int client_addr_size_;
};