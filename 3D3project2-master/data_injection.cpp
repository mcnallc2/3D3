#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <chrono>
#include <mutex>
#include "router_header.h"

using namespace std;

//args 1: first router 2: destination 3: data
//message "Data: (first router name) (destination router name) (message)"
int main(int argc, char ** argv) {
	if (argc != 4) {
		perror("Need 3 arguments");
		//return 1;
	}

	string src = argv[1];
	string dest = argv[2];
	string data;
	for (int i = 3; i < argc; i++){
		data = data + " " + argv[i];
	}
	string message = DATA_STRING + string(" src: ")+ string(src) + string(" dest: ") + string(dest) + string(" data: ") + string(data);

	//create socket
	int sockfd; 
    struct sockaddr_in addr; 

  	int port = **(argv + 1) + 9935;	//use the right port

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) { 
        perror("socket creation failed"); 
    }

    memset(&addr, 0, sizeof(addr));   
	//set the adressse to the right port
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port); 
    addr.sin_addr.s_addr = INADDR_ANY; 

	cout << "message " <<message << "\nsending to port " << port << endl;
	if (sendto(sockfd, message.c_str(), message.length(), MSG_CONFIRM, (const struct sockaddr*)&addr, sizeof(addr)) == -1){
		perror("error sending in data injection : ");
	}
	return 0;
}
