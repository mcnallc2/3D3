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

#include "HttpRequest.h"

using namespace std;

int
main(int argc, char *argv[])
{
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
  cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << endl;


  // send/receive data to/from connection
  bool isEnd = false;
  string input;
  char buf[100] = {0};
  stringstream ss;

  HttpRequest single_request;

  single_request.setMethod("GET ");
  single_request.setURI(argv[1]);
  single_request.setVersion(" HTTP/1.0\n");
  single_request.setHeader("User-Agent: Ubuntu 7.3.0-27ubuntu1~18.04\nHOST: web-client.cpp\n");
  single_request.setBody("I'M TRYING MY BEST HERE!!!\n");

  string http_request;
  http_request = single_request.getMethod() + single_request.getURI() + single_request.getVersion() + single_request.getHeader() + single_request.getBody();


  cout << "Requesting URI...";

  if (send(sockfd, http_request.c_str(), http_request.size(), 0) == -1) {
    perror("send");
    return 4;
  }
 

  while (!isEnd) {

    //clears the buffer by replacing full buffer with null(\0)
    memset(buf, '\0', sizeof(buf));

    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }
    ss << buf << endl;
    cout << "echo: ";
    cout << buf << endl;

    if (ss.str() == "close\n")
      break;

    ss.str("");

    cout << "send: ";
    cin >> input;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }
  }

  close(sockfd);

  return 0;
}

