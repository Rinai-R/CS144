#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL(const string &host, const string &path) {
  cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  // 通过域名解析获取 ip
  struct hostent *server = gethostbyname(host.c_str());
  if (server == NULL) {
    cerr << "Error: Could not get host information for " << host << "\n";
    return;
  }
  // 这是一个用来表示 ipv4 地址信息的结构体
  // 用于套接字编程
  struct sockaddr_in server_addr;
  // 填充这个结构体
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = *(u_long *)server->h_addr_list[0];
  server_addr.sin_port = htons(80);
  // 本地创建一个套接字
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    cerr << "Error: Could not create socket\n";
    return;
  }
  // 连接到远程服务器
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    cerr << "Error: Could not connect to server\n";
    return;
  }
  string request = "GET " + path + " HTTP/1.1\r\nHost: " + host +
                   "\r\nConnection: close\r\n\r\n";
  if (send(sockfd, request.c_str(), request.size(), 0) < 0) {
    cerr << "Error: Could not send request\n";
    return;
  }
  char buffer[1024];
  int bytes_read = 0;
  while ((bytes_read = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
    write(1, buffer, bytes_read);
  }
  if (bytes_read < 0) {
    cerr << "Error: Could not read response\n";
    return;
  }
  close(sockfd);
}

int main(int argc, char *argv[]) {
  try {
    if (argc <= 0) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span(argv, argc);

    // The program takes two command-line arguments: the hostname and "path"
    // part of the URL. Print the usage message unless there are these two
    // arguments (plus the program name itself, so arg count = 3 in total).
    if (argc != 3) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host{args[1]};
    const string path{args[2]};

    // Call the student-written function.
    get_URL(host, path);
  } catch (const exception &e) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}