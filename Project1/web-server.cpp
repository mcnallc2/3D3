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
#include <stdlib.h>


#include "HttpResponse.hpp"
#include "HttpResponse.cpp"

#define BUFFER_SIZE 1000
#define A_SIZE 100

void http_response(int client_socket, char* buffer);
string request_parcer(char buffer[]);
string uri_parcer(char uri[]);
void next_field(FILE *f, char *buf, int max);

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
  cout << "\nRequest recieved ...\n\n" << endl;
  cout << "Looking for file: " << request_parcer(buf);

  string status;
  // Try to open the input file. If there is a problem, report failure and quit
  FILE *f;
	f = fopen(request_parcer(buf).c_str(), "r");
	if(!f) { 
		printf("unable to open %s\n", request_parcer(buf).c_str()); 
		status = "404 Not Found\n";
    f = fopen("404.html", "r");
  }

  char* html_text;
  next_field(f, html_text, BUFFER_SIZE);

  cout << html_text;
  


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

string request_parcer(char *buffer, string version, string uri){

  string method;
  string uri;
  string version;
  string header;
  string body;

  char temp[A_SIZE];
  string parce_buffer = buffer;

  memset(temp, '\0', A_SIZE);
  int i = 0;
  while(parce_buffer[i] != ' '){
    temp[i] = buffer[i];
    i++;
  }
  method = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  int j=0;
  while(parce_buffer[i] != ' '){
    temp[j] = buffer[i];
    i++;
    j++;
  }
  uri = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(buffer[i] != '\n'){
    temp[j] = buffer[i];
    i++;
    j++;
  }
  version = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(buffer[i] != '\n'){
    temp[j] = buffer[i];
    i++;
    j++;
  }
  header = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(buffer[i] != '\n'){
    temp[j] = buffer[i];
    i++;
    j++;
  }
  body = temp;

  return uri;
}

void next_field( FILE *f, char *buf, int max ) {

	int i = 0;
	
	for(;;) {

		// fetch the next character from file		
		buf[i] = fgetc(f);

		// end record on newline or end of file
		if(feof(f) || buf[i]=='\n'){
			break;
		} 

		// truncate fields that would overflow the buffer
		if( i<max-1 ){
			++i;
		} 
	}

	buf[i] = 0; // null terminate the string
}
