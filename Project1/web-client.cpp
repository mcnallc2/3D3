// #include <string>
// #include <thread>
// #include <iostream>

// int main()
// {
//   cerr << "web client is not implemented yet" << endl;
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

#include "HttpRequest.hpp"
#include "HttpRequest.cpp"
#define BUFFER_SIZE 1000

using namespace std;

void send_request(int socket, string http_request, char buffer[]);

int
main(int argc, char *argv[])
{
  //creating the object for the http request
  HttpRequest* first_request = new HttpRequest("GET ", argv[1], " HTTP/1.0\n", "HOST: web-client.cpp\n", "TRYING MY BEST HERE!!!\n");

  string http_request;
  http_request = first_request->getMethod() + first_request->getURI() + first_request->getVersion() + first_request->getHeader() + first_request->getBody();
  //cout << http_request << endl;

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(40000);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // send/receive data to/from connection
  bool isEnd = false;
  std::string input;
  char buf[BUFFER_SIZE] = {0};
  std::stringstream ss;

  send_request(sockfd, http_request, buf);

  close(sockfd);

  return 0;
}

void send_request(int socket, string http_request, char buffer[]){

  if (send(socket, http_request.c_str(), http_request.size(), 0) == -1) {
    perror("send");
    //return 4;
  }

  memset(buffer, '\0', sizeof(buffer));
  if (recv(socket, buffer, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    //return 5;
  }

  //ss << buffer << endl;
  cout << buffer << endl;
}

