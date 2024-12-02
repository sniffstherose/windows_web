#include "client.h"
#include "server.h"
#include <getopt.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << "<server|echo> <args...>" << std::endl;
    return -1;
  }

  // 保存命令行参数的变量
  std::string mode = static_cast<std::string>(argv[1]);
  int connection_count = 0;
  std::string ip{};
  int port{};

  // 解析参数
  if (mode == "server") {
    port = std::stoi(static_cast<std::string>(argv[2]));
    TcpTestServer serv(port);
    serv.run();
  } else {
    int cnt = atoi(argv[2]);
    std::string ip = static_cast<std::string>(argv[3]);
    int port = atoi(argv[4]);
    TcpTestClient clie(cnt, ip, port);
    clie.run();
  }

  return 0;
}
