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
#define A_SIZE 100

void http_response(int client_socket, char* buffer);
string request_parcer(char buffer[]);
string uri_parcer(char uri[]);

int
main(int argc, char *argv[])
{
  string host = argv[1];
  string port_number = argv[2];
  string directory = argv[3];

  if(host != "localhost"){
    cout << "host has been changed to 'localhost'\n";
    host = "localhost";
  }
  string ip_address = "127.0.0.1";
  int port = atoi(port_number.c_str());

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
  addr.sin_port = htons(port);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    cout << "\nbind error\n";
    perror("bind");
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    cout << "\nlisten error\n";
    perror("listen");
  }

  // accept a new connection
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  cout << "Accept a connection from: " << ipstr << ":" << ntohs(clientAddr.sin_port) << endl;


  char buf[BUFFER_SIZE];
  stringstream ss;

  memset(buf, '\0', sizeof(buf));
  if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    return 5;
  }
  cout << "\nRequest recieved ...\n\n" << buf << endl;

  request_parcer(buf);

  http_response(clientSockfd, buf);

  //ss << buf << endl;
  //cout << buf << endl;
  // if (ss.str() == "close\n")
  // ss.str("");

  close(clientSockfd);

  return 0;
}

void http_response(int client_socket, char* buffer){

  if (send(client_socket, buffer, BUFFER_SIZE, 0) == -1) {
    perror("send");
    //return 6;
  }
}

string request_parcer(char *buffer){

  char method[A_SIZE];
  char uri[A_SIZE];
  char version[A_SIZE];
  char header[A_SIZE];
  char body[A_SIZE];

  string http_response;

  int i = 0;
  while(buffer[i] != ' '){
    method[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != ' '){
    uri[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != '\n'){
    version[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != '\n'){
    header[i] = buffer[i];
    i++;
  }
  i++;
  while(buffer[i] != '\n'){
    body[i] = buffer[i];
    i++;
  }
  return uri;
}
