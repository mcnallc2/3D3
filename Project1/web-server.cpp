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

#define BUFFER_SIZE 2000
#define A_SIZE 100

void http_response(int client_socket, string response);
string request_parcer(char buffer[]);
string uri_parcer(char uri[]);
void next_field(FILE *f, char *buf, int max);

string uri, version;

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
  cout << endl << "Accept a connection from: " << ipstr << ":" << ntohs(clientAddr.sin_port) << endl;


  char buf[BUFFER_SIZE];
  stringstream ss;

  memset(buf, '\0', sizeof(buf));
  if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    return 5;
  }
  cout << endl << buf << endl;

  request_parcer(buf);

  cout << "Request recieved ...\n\n";
  cout << "Looking for file: " << uri << endl;
  cout << "Client is using " << version << endl;

  string status;
  // Try to open the input file. If there is a problem, report failure and quit
  FILE *f;
	f = fopen(uri.c_str(), "r");
	if(!f) { 
		printf("unable to find %s\n", uri.c_str()); 
		status = "404 Not Found";
    f = fopen("404.html", "r");
  }
  else if(strcmp(version.c_str(), "HTTP/1.1") != 0){
    printf("incorrect version"); 
		status = "400 Bad Request";
    f = fopen("400.html", "r");
  }
  else{
    status = "200 OK";
  }


  char html_text[BUFFER_SIZE];
  next_field(f, html_text, BUFFER_SIZE);

  HttpResponse* first_response = new HttpResponse(version, status, "Date: 28th Feb 2019\nServer: web-server.cpp\n\n", html_text);	
  
  string http_response;
  http_response = first_response->getVersion() + " " + first_response->getStatus() + "\n" + first_response->getHeader() + first_response->getHTML();

  cout << endl << http_response;
  if (send(clientSockfd, http_response.c_str(), BUFFER_SIZE, 0) == -1) {
    perror("send");
    //return 6;
  }
  //http_response(clientSockfd, http_response);

  //ss << buf << endl;
  //cout << buf << endl;
  // if (ss.str() == "close\n")
  // ss.str("");

  close(clientSockfd);

  return 0;
}

void http_response(int client_socket, string response){

  if (send(client_socket, response.c_str(), BUFFER_SIZE, 0) == -1) {
    perror("send");
    //return 6;
  }
}

string request_parcer(char *buffer){

  string method;
  string header;
  string body;

  char temp[A_SIZE];
  string parce_buffer = buffer;

  memset(temp, '\0', A_SIZE);
  int i = 0;
  while(parce_buffer[i] != '/'){
    temp[i] = parce_buffer[i];
    i++;
  }
  method = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  int j=0;
  while(parce_buffer[i] != ' '){
    temp[j] = parce_buffer[i];
    i++;
    j++;
  }
  uri = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(parce_buffer[i] != '\r'){
    temp[j] = parce_buffer[i];
    i++;
    j++;
  }
  version = temp;
  i++;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while(parce_buffer[i] != '\n'){
    temp[j] = parce_buffer[i];
    i++;
    j++;
  }
  header = temp;
  i++;

  memset(temp, '\0', A_SIZE);
  j=0;
  while((parce_buffer[i] != '\n') && (parce_buffer[i+1] != '\n')){
    temp[j] = parce_buffer[i];
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
		if(feof(f)){
			break;
		} 

		// truncate fields that would overflow the buffer
		if( i<max-1 ){
			++i;
		} 
	}

	buf[i] = 0; // null terminate the string
}
