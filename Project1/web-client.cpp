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
void next_field(FILE *f, char* buf, int max);
void parce_response(string response);

string host, port_number, html, ip_address;
char received_html[BUFFER_SIZE];
int next_arg;
string argument;

int
main(int argc, char *argv[])
{

  next_arg = 1;
  argument = "|";

  //parcing url
  parce_argument(argv[next_arg]);

  ip_address = "127.0.0.1";
  string body = "User-Agent: web-client.cpp (X11; Ubuntu; Linux x86_64; rv:65.0)\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5\nAccept-Encoding: gzip, deflate\nConnection: keep-alive\nUpgrade-Insecure-Requests: 1\n\n";

  //converting port number to int and setting header
  int port = atoi(port_number.c_str());
  string header = host + ":" + port_number;

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  //struct for socket attributes
  struct sockaddr_in serverAddr;
  //specifies ip version (ipv4)
  serverAddr.sin_family = AF_INET;
  //converting port from host to network form (short)
  serverAddr.sin_port = htons(port);
  //combines the internet ip and host ip
  serverAddr.sin_addr.s_addr = inet_addr(ip_address.c_str());
  //sets the ip address char version to NULL
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  //connecting to the server to the server using given port
  if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
    perror("connect");
  }

  //struct for client socket
  struct sockaddr_in clientAddr;
  //attribute for length of socket
  socklen_t clientAddrLen = sizeof(clientAddr);
  //stores the local host address and length
  if(getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
  }

  string http_request;
  char buf[BUFFER_SIZE] = {0};

  //while(next_arg < argc){

    
  parce_argument(argv[next_arg]);
  port = atoi(port_number.c_str());
  header = host + ":" + port_number;
  //displaying the ip address and network to host short verion of port number
  cout << "\nSet up a connection from: " << ip_address << ": " << ntohs(clientAddr.sin_port) << endl << endl;

  //creating the object for the http request
  HttpRequest* first_request = new HttpRequest("GET ", html, " HTTP/1.1\r\n", header, body);

  //using the object construct the http request
  http_request = first_request->getMethod() + "/" + first_request->getURI() + first_request->getVersion() + "Host: " + first_request->getHeader() + "\n" + first_request->getBody();
  delete first_request;

  //function to send the request
  cout << "Sending Request...\n\n";
  send_request(sockfd, http_request, buf);

  next_arg++;
  //argument = argument + "|";


  //}
  //sending the http_request

  // http_request = "finish";
  // send_request(sockfd, http_request, buf);


  close(sockfd);

  return 0;
}

void send_request(int socket, string http_request, char* buffer){

  int flag = 0;
  string file_name;
  //sending the http_request
  if(send(socket, http_request.c_str(), http_request.size(), 0) == -1){
    perror("send");
  }

  //setting buffer and recieving the response from server
  memset(buffer, '\0', BUFFER_SIZE);
  if(recv(socket, buffer, BUFFER_SIZE, 0) == -1){
    perror("recv");
  }
  cout << endl << buffer << endl;

  if(strcmp(buffer, "finish") == 0){
    cout << endl << "server closing" << endl;
    flag = 1;
  }

  if(flag == 0){
    //parcing the http response
    parce_response(buffer);

    //opening new file and writing in the response from the server
    FILE *f;
    //file_name = "recieved" + argument + ".html";
    f = fopen("recieved.html", "w");
    //cout << received_html;
    next_field(f, received_html, BUFFER_SIZE);
  }


}

//function to parse the input argument
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

//function to write string to file
void next_field( FILE *f, char* buf, int max) {

	int i = 0;
	
	for(;;) {
   // short, network byte order
		//put the next character in the file		
    fputc(buf[i], f);
		//end when reached 2 endlines
		if(buf[i] == '\n' && buf[i+1] == '\n'){
			break;
		} 
		//truncate fields that would overflow the buffer
		if( i<max-1 ){
			++i;
		} 
	}
	buf[i] = 0; // null terminate the string
}

//function to parce the response fro mthe server
void parce_response(string response){

  //skipping status and header
  int i=0;
  while(response[i] != '<'){
    i++;
  }

  //obtaining string of html code
  int j=0;
  while(response[i] != '\0'){
    received_html[j] = response[i];
    i++;
    j++;
  }
}