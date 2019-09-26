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

  //ensuring localhost is used
  if(host != "localhost"){
    cout << "host has been changed to 'localhost'\n";
    host = "localhost";
  }
  string ip_address = "127.0.0.1";
  //converting port to int
  int port = atoi(port_number.c_str());

  //create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  //allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
  }
 
  struct sockaddr_in addr;
  //struct for socket attributes
  addr.sin_family = AF_INET;
  //converting port from host to network form (short)
  addr.sin_port = htons(port);
  //combines the internet ip and host ip
  addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
   //sets the ip address char version to NULL
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));


  //bind address to socket
  if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    cout << "\nbind error\n";
    perror("bind");
  }

  char buf[BUFFER_SIZE];
  string status;
  FILE *f;
  char html_text[BUFFER_SIZE];
  string http_response;
  int clientSockfd;
  //accept a new connection from client
  struct sockaddr_in clientAddr;
  //fetching clent address size
  socklen_t clientAddrSize = sizeof(clientAddr);


  //while(true){


  //set socket to listen status
  if(listen(sockfd, 1) == -1) {
    cout << "\nlisten error\n";
    perror("listen");
  }

  //used to thread the incoming requests
  //fork();

  //await a connection from client and setting the socket descriptor name and size to that of client
  clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  //if desriptor is -1 there is and error opening the connection
  if (clientSockfd == -1) {
    perror("accept");
  }

  //displaying ip address and network to host (short) version of port (non-binary version)
  cout << endl << "Accept a connection from: " << ip_address << ": " << ntohs(clientAddr.sin_port) << endl;

  memset(buf, '\0', sizeof(buf));
  //receiving request from client and storing in buffer
  if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
    perror("recv");
  }
  cout << endl << buf << endl;

  //if there are no more requests
  // if(strcmp(buf, "finish") == 0){
  //   break;
  // }

  //function to parce request from client
  request_parcer(buf);

  cout << "Request for (" << uri << ") recieved." << endl;
  cout << "Client is running with " << version << endl;

  //trying to open file request from the client
  f = fopen(uri.c_str(), "r");
  //if the file is not found
  if(!f) { 
    printf("unable to find %s\n", uri.c_str()); 
    status = "404 Not Found";
    f = fopen("404.html", "r");
  }
  //if the http version is incorrect
  else if(strcmp(version.c_str(), "HTTP/1.1") != 0){
    printf("incorrect version"); 
    status = "400 Bad Request";
    f = fopen("400.html", "r");
  }
  //else file is found
  else{
    status = "200 OK";
  }

  //function to read html code from fequested file
  next_field(f, html_text, BUFFER_SIZE);

  //creating object for the request
  HttpResponse* first_response = new HttpResponse(version, status, "Date: 28th Feb 2019\nServer: web-server.cpp\n\n", html_text);	
  
  //using the object to construct a response to the client
  http_response = first_response->getVersion() + " " + first_response->getStatus() + "\n" + first_response->getHeader() + first_response->getHTML() + "\n\n";
  delete first_response;

  cout << endl << "Sending Response...\n" << endl;
  //sending response to the client
  if (send(clientSockfd, http_response.c_str(), BUFFER_SIZE, 0) == -1) {
    perror("send");
  }

  // }
  // http_response = "finish";
  // if (send(clientSockfd, http_response.c_str(), BUFFER_SIZE, 0) == -1) {
  //   perror("send");
  // }

  close(clientSockfd);

  return 0;
}

//function to parce the request from client
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

//function to read html code from the requested file
void next_field( FILE *f, char *buf, int max ) {

	int i = 0;
	
	for(;;) {

		//fetch the next character from file		
		buf[i] = fgetc(f);

		//end record at end of file
		if(feof(f)){
			break;
		} 
		//truncate fields that would overflow the buffer
		if( i<max-1 ){
			++i;
		} 
	}

	buf[i] = 0;// null terminate the string
}
