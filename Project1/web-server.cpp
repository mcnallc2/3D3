// #include <string>
// #include <thread>
// #include <iostream>

// int main()
// {
//   std::cerr << "web server is not implemented yet" << std::endl;
//   // do your stuff here! or not if you don't want to.
// }

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "HttpResponse.hpp"
#include "HttpResponse.cpp"

#define BUFFER_SIZE 1000

void http_response(int client_socket, char buffer[]);
string request_parcer(char buffer[]);

int
main(int argc, char *argv[])
{
  host = argv[1];
  port_number = argv[2]
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(40000);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // accept a new connection
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // read/write data from/into the connection
  bool isEnd = false;
  char buf[BUFFER_SIZE] = {0};
  std::stringstream ss;


  memset(buf, '\0', sizeof(buf));
  if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    return 5;
  }

  http_response(clientSockfd, buf);

  //ss << buf << endl;
  cout << buf << endl;
  // if (ss.str() == "close\n")
  // ss.str("");

  close(clientSockfd);

  return 0;
}

void http_response(int client_socket, char buffer[]){

  if (send(clientSockfd, request_parcer(buffer), BUFFER_SIZE, 0) == -1) {
    perror("send");
    //return 6;
  }
}

string request_parcer(char buffer[]){

  char method[A_SIZE];
  char uri[A_SIZE];
  char version[A_SIZE];
  char header[A_SIZE];
  char body[A_SIZE];

  int status_code;
  string http_response;

  int i = 0;
  while(buffer[i] != " "){
    method[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != " "){
    uri[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != "\n"){
    version[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != "\n"){
    header[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != "\n"){
    body[i] = buffer[i];
    i++;
  }

  if(version != "HTTP/1.0"){
    status_code = 3;
  }
  if(url_lookup(uri)){
    status code = 1;
  }

  http_response = version +
}
