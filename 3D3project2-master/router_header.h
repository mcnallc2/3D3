#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <sstream>
#include <vector>
#define MAX_SIZE 26
#define SEND_DELAY 5
#define DATA_STRING "DATA"
#define CONTROL_STRING "CONTROL"
#define DEAD_STRING "DEAD"
#define DEAD_CHAR "x"
#define UNDEFINED_ROUTER_CHAR "."
#define DEAD_TIME 10	//this is the number of seconds before router is declared dead

extern int new_routers;

using namespace std;

class neighbour {
public:
	int port_num;
	string name;
	int distance;
	bool connected;
};

class table {
public:
	string predecessor[MAX_SIZE];
	string destination[MAX_SIZE];
	string next[MAX_SIZE];
	int distance[MAX_SIZE];
};

class router {
public:
	int port_num;
	int sockfd;
	bool is_new;
	string name;
	neighbour neighbours[MAX_SIZE];
	table forwarding;
	string new_lines[2 * MAX_SIZE];
	int neighbour_costs[MAX_SIZE];

	vector<int> router_count;
	vector<char> router_id;
	// contains the id of the routers at each index in the router_count vector


	vector<int> router_time = vector<int>(MAX_SIZE, 0); 
	//initial value of 0
	//if the router is known to be dead, then have a value of -1
	//otherwise track the time that the last update was recieved

	router(int p, string n, string * console_input);
	int bind_r();
	void go_online();
	void send_update(char killing_router); // set char to name of router if youre sending updates that a router has died
	void check_router_count(char updating_router, bool periodic_refresh);
	int send_forward(string dest, string message);
	void handleDeadRouters(int router_index);
};

void printTable(int table[], char src_router, string next_hops[]) ;
void receive_th(router &r, struct sockaddr_in addr);
void send_th(router &r);