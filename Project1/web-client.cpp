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
#define BUFFER_SIZE 2000
#define A_SIZE 100

using namespace std;

void send_request(int socket, string http_request, char* buffer);
void parce_argument(string input_url);

string host, port_number, html, ip_address;

int
main(int argc, char *argv[])
{
  parce_argument(argv[1]);

  ip_address = "127.0.0.1";
  string body = "User-Agent: web-client.cpp (X11; Ubuntu; Linux x86_64; rv:65.0)\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5\nAccept-Encoding: gzip, deflate\nConnection: keep-alive\nUpgrade-Insecure-Requests: 1\n\n";

  int port = atoi(port_number.c_str());

  //creating the object for the http request
  HttpRequest* first_request = new HttpRequest("GET ", html, " HTTP/1.1\r\n", host, body);

  string http_request;
  http_request = first_request->getMethod() + "/" + first_request->getURI() + first_request->getVersion() + "Host: " + first_request->getHeader() + ":" + port + "\n" + first_request->getBody();
  //cout << http_request << endl;

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr(ip_address.c_str());
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" << ntohs(clientAddr.sin_port) << endl << endl;

  std::string input;
  char buf[BUFFER_SIZE] = {0};
  std::stringstream ss;

  send_request(sockfd, http_request, buf);

  close(sockfd);

  return 0;
}

void send_request(int socket, string http_request, char* buffer){

  if (send(socket, http_request.c_str(), http_request.size(), 0) == -1) {
    perror("send");
    //return 4;
  }

  memset(buffer, '\0', BUFFER_SIZE);
  if (recv(socket, buffer, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    //return 5;
  }

  //ss << buffer << endl;
  cout << buffer << endl;
}

void parce_argument(string input_url){

  char temp[A_SIZE];

  int i=0;
  while(input_url[i] != '/'){
    i++;
  }
  i++;
  i++;

  memset(temp, '\0', A_SIZE);
  int j=0;
  while(input_url[i] != ':'){
    temp[j] = input_url[i];
    i++;
    j++;
  }
  i++;
  host = temp;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(input_url[i] != '/'){
    temp[j] = input_url[i];
    i++;
    j++;
  }
  i++;
  port_number = temp;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(input_url[i] != '\0'){
    temp[j] = input_url[i];
    i++;
    j++;
  }
  html = temp;
}