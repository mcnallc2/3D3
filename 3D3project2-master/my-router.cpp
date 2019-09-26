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

int new_routers = 20;

int main(int argc, char ** argv) {

	

	//take command line info
	string name = argv[1];
	int port = **(argv + 1) + 9935;
	//int * data = new int[argc - 1];
	string * console_input= new string [argc - 1];
	//string console_input[10];
	 
	if(argc == 0){
		cout << "No argument entered\n";
		return 0;
	}
	else if(argc > 1){
		int i = 2;

		while(i < argc){
			console_input[i - 2] = argv[i];
			i++;
		}
	}
	router r(port, name, console_input);
	r.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	

	//set socket as reusable
	int yes = 1;
	if (setsockopt(r.sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		return 1;
	}

	//set timeout
	struct timeval tv;
	tv.tv_sec = 10;  /* 30 Secs Timeout */
	setsockopt(r.sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	//bind this router to the socket
	r.bind_r();

	if(r.is_new){
		new_routers--;
		r.go_online();
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;     // short, network byte order
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(r.port_num);
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));


	//set up thread to manage recieving updates
	thread t_recieve(receive_th, ref(r), addr);
	//set up thread to manage sending periodic updates
	thread t2(send_th, ref(r));
	t2.join();	//this is blocking  and stops main thread from terminating

	return 0;
}