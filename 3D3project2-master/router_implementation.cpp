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
#include <iomanip>
#include <algorithm>

using namespace std;
#include "router_header.h"

//-----------------------------router related sending stuff ----------------------------------------

router::router(int p, string n, string* console_input) {
	fstream file;
	file.open("nodes.txt", ios::in);
	port_num = p;
	name = n;

	is_new = false;
	//neighbour port number
	int neighbour_num;
	string buffer;

	//initialise array
	//all port numbers are defined and each router is not connected
	for (int i = 0; i < MAX_SIZE; i++){
		neighbours[i].connected = false;
		neighbours[i].port_num = i + 10000;
	}
	
	//include info in neighbours array for self
	neighbours[port_num - 10000].port_num = port_num;
	neighbours[port_num - 10000].distance = 0;
	neighbours[port_num - 10000].name = name;
	neighbours[port_num - 10000].connected = false;


	//this branch is taken if there is input from the command line (this router is new)
	if(console_input[0].length() != 0){
		//i increments in twos as the format is A 1 B 2 C 3 for connection input from command line 
		for(int i = 0; console_input[i].length() != 0; i = i + 2){
			int index = console_input[i][0] - 65;
			//convert port number to char
			neighbours[index].port_num = console_input[i][0] + 9935;
			neighbours[index].distance =  atoi(console_input[i + 1].c_str());
			neighbours[index].name = console_input[i];
			neighbours[index].connected = true;
			//if the router is a new router 
			if(port_num >10005)
				is_new = true;
		}
	}
	else{
		while (!file.eof()){

			getline(file, buffer);
			//if the buffer has info relating to this node
			if (buffer.substr(0, 1) == name){

				//store port number
				neighbour_num = stoi(buffer.substr(4, 5));
				//store name in neighbour array
				neighbours[neighbour_num - 10000].name = buffer.substr(2, 1);
				//store port number in neighbour array				cout <<"not connected" <<forwarding.destination[i] << endl;
				neighbours[neighbour_num - 10000].port_num = neighbour_num;
				//store distance in neighbour array
				neighbours[neighbour_num - 10000].distance = stoi(buffer.substr(buffer.find_last_of(buffer) - 1, 1));

				neighbours[neighbour_num - 10000].connected = true;
			}
		}
		file.close();
	}

	//store direct connections into routing table
	for (int i = 0; i < MAX_SIZE; i++){
		if (i == port_num - 10000){
			forwarding.distance[i] = 0;
			forwarding.destination[i] = name;
			forwarding.next[i] = name;
		}
		else if(neighbours[i].connected){
			forwarding.distance[i] = neighbours[i].distance;
			forwarding.destination[i] = neighbours[i].name;
			forwarding.next[i] = neighbours[i].name;

			neighbour_costs[i] = neighbours[i].distance;
		}
		else{
			//no entry for this node at present
			forwarding.destination[i] = (char)i + 65;
			forwarding.distance[i] = 9999;
			forwarding.next[i] = UNDEFINED_ROUTER_CHAR;
		}
	}
}

void router::go_online(){
	struct sockaddr_in addr;
	socklen_t length = sizeof(addr);
	string buf;
	addr.sin_family = AF_INET;     // short, network byte order
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	//this loop is only used when the router being created in new
	//in order to connect with the other nodes, this router needs to send its 
	//connection info about A to A, B to B, C to C etc. before it sends the nodes any other routing info
	//e.g, if the first message a new node "G" sends to B is about A the forwarding tables will be wrong
	for (int i = 0; i < MAX_SIZE - new_routers; i++){
		if(neighbours[i].connected){
			addr.sin_port = htons(neighbours[i].port_num);
			buf = CONTROL_STRING + string(" ") + name + "\n\0";

			if (sendto(sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&addr, length) == -1) {
				perror("sending update");
			}
			buf = "Destination: " + forwarding.destination[i] + " Next hop: " + forwarding.next[i] + " Distance " + to_string(forwarding.distance[i]) + "\n\0";
	
			if (sendto(sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&addr, length) == -1) {
				perror("Error sending to particular destination: ");
			}
		}
	}
}


void router::send_update(char dead_router) {	//dead router is the name of the router which has died
	struct sockaddr_in addr;
	socklen_t length = sizeof(addr);
	string buf;
	addr.sin_family = AF_INET;     // short, network byte order
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	//sends routing information if router is connected to a particular node
	for (int i = 0; i < MAX_SIZE - new_routers; i++){
		if (neighbours[i].connected){
			//set sending address to port number of this neighbour (i)
			addr.sin_port = htons(neighbours[i].port_num);
			
			if(!dead_router){ //send usual control data
				// send header
				buf = CONTROL_STRING + string(" ") + name + "\n\0";
				if (sendto(sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&addr, length) == -1) {
					perror("sending update");
				}
				for (int j = 0; j < MAX_SIZE - new_routers; j++){
					//if there is an entry in the forwarding tabe that is not this router

					buf = "Destination: " + forwarding.destination[j] + " Next hop: " + forwarding.next[j] + " Distance " + to_string(forwarding.distance[j]) + "\n\0";
					if (sendto(sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&addr, length) == -1) {
							perror("Error sending to particular destination: ");
					}
					buf = "";
				}
			}
			else{
				//send update to all connected neighbours about the dead router
				buf = DEAD_STRING + string(" ") + dead_router + "\n\0;";
				if (sendto(sockfd, buf.c_str(), buf.length(), 0, (struct sockaddr*)&addr, length) == -1) {
					perror("sending update about dead router:");
				}
			}
		}
	}
}

int router::bind_r() {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_num);     // short, network byte order
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	//bind to socket using info
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind");
		return 2;
	}
	return 1;
}

void router::check_router_count(char updating_router, bool periodic_refresh){
	//take in the id of the router tahat is sending updates as a parameter
	int wait_time = 2* DEAD_TIME;	//if you are checking for lost routers with periodic refreshing,
									// then check they have been inactive for a longer time period

	int index = updating_router - 65;
	int now = time(0);
	if(!periodic_refresh){
		router_time[index] = now;
		wait_time = DEAD_TIME;	
	}

	for (int i = 0; i < MAX_SIZE - new_routers; i++){
		if(router_time[i] >0  && router_time[i] + wait_time < now ){
			printf( "killing router %c, with a count of %d compared to current time %d \n", char(i+65), router_time[i],now);
			//if a router 'Y' finds that it's neighbouring router 'Z' dies, we tell all its neighbours to delete any entries
			//in their tables that have router 'Z' as destination or as next hop - the handleDeadRouters function does all this
			handleDeadRouters(i);
		}
	}
}

void router::handleDeadRouters(int router_index){
	//handling dead routers: set the distance from this router to the dead router to have a value of 9999
	// set the value of all the paths that have that node as the next hop to be 9999

	string dead_router_name = string(1, (char)(router_index + 65));

	if(router_time[router_index] != -1 ){	//if this router has not already recieved the information about the dead_router, then:
		router_time[router_index] = -1;		//mark the router as dead
		forwarding.distance[router_index] = 9999;	//set its distance to infinte
		forwarding.next[router_index] = DEAD_CHAR;
		cout << "\t Handled dead router " + dead_router_name << endl;
		
		send_update(dead_router_name.at(0));
	}
	bool all_connected_routers_dead = true;	 //if all the routers connected to this router are dead
											// then it should advertise that it is dead
	for(int i = 0; i < MAX_SIZE - new_routers; i++){
		if(forwarding.next[i] == dead_router_name){
			forwarding.next[i] = DEAD_CHAR;
			forwarding.distance[i] = 9999;
		}
		if(router_time[i] != -1 && neighbours[i].connected)all_connected_routers_dead = false;
	}
	if(all_connected_routers_dead){
		cout << "all connected routers dead!" << endl;
	}

	printTable(forwarding.distance, name.at(0), forwarding.next);

}

int router::send_forward(string dest, string message) {
	struct sockaddr_in addr;
	socklen_t length = sizeof(addr);
	addr.sin_family = AF_INET;     // short, network byte order
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
	int i = 0;
	int dest_port;

	while (i < MAX_SIZE - new_routers)
	{	//search forwarding table for destination
		//when found, take the next hop and forward message to it
		if (dest == forwarding.destination[i]){
			dest_port = (int)forwarding.next[i][0] + 9935;
			break;
		}
		else{
			i++;
		}
	}

	addr.sin_port = htons(dest_port);

	if (sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&addr, length) == -1){
		perror("sending update");
	}
	return dest_port;
}

//-----------------------------------------------------------------parsing and thread stuff below this -----------

void printTable(int table[], char src_router, string next_hops[]) {
	cout << ":FORWARDING TABLE:" << endl;
	for (int i = 0; i < MAX_SIZE - new_routers; i++) {
		cout << src_router << "  ->  " << (char)(i + 65) << setw(5) << table[i] << "  [next = " << next_hops[i] << "]" << endl;
	}
}

void printToFile(string lines[], string filechar, router &r, bool before) {
	ofstream routerfile;
retry_open:
	routerfile.open("routing-output" + filechar + ".txt", std::ios_base::app | std::ios_base::out);
	if (!routerfile.is_open())goto retry_open;
	if(before){	
		time_t _tm = time(NULL);
		struct tm * curtime = localtime(&_tm);
		string time = asctime(curtime);
		routerfile << "\n\ncurrent time: " << time << endl;
		routerfile << "routing table before change: " << endl;
		for (int i = 0; i < MAX_SIZE - new_routers; i++){
			routerfile << r.name << "  ->  " << (char)(i + 65) << setw(5) << r.forwarding.distance[i] << "  [next = " << r.forwarding.next[i] << "]" << endl;
		}
		routerfile << "recieved information causing the change:" << endl;
		for (int i = 0; i< 2 * MAX_SIZE - new_routers; i++) {
			routerfile << lines[i];
		}
	}else{
		routerfile << "table after the change: " << endl;
		for (int j = 0; j < MAX_SIZE - new_routers; j++){
			routerfile << r.name << "  ->  " << (char)(j + 65) << setw(5) << r.forwarding.distance[j] << "  [next = " << r.forwarding.next[j] << "]" << endl;
		}
	}

	routerfile.close();
}

void printToFile_Forwarding(string forward_data, int arrival_port, int source_port, string arrival_name, string source_name) {
    ofstream forwardingfile;
retry_open:
    forwardingfile.open("forwarding-output.txt", std::ios_base::app | std::ios_base::out);
    if (!forwardingfile.is_open())goto retry_open;
    //compare to see if the new_lines we recieved are the same as the old ones
    forwardingfile << "Source Port: " << source_port << " - " << source_name << endl << "Arrival Port: " << arrival_port << " - " << arrival_name << endl << forward_data << endl;
    forwardingfile.close();
}

void update(router &r) {
	int i = 0, k = 0;
	string update_from, dest, current_line, next_hop, delim1 = "CONTROL ", delim2 = "Destination: ";
	string delim3 = "Next hop: ", delim4 = "Distance: ";
	string all_info;
	int new_distance, index_update, index_dest;

	//find how many lines are in array
	while (r.new_lines[k].length() != 0){
		k++;
	}

	//take first line 
	if (k > 0) {
		update_from = r.new_lines[0].substr(r.new_lines[0].find(delim1) + delim1.length(), 1);
		//here we can update the router count for the router we just recieved from
		//r.router_count[(update_from.at(0) - 65)] += 1;
		r.check_router_count(update_from.at(0), false);
		//all_info.erase(0, all_info.find(delim1) + delim1.length() + 1);
	}

	i = 1;
	while (i < k) {
		try{
			all_info = r.new_lines[i];

			dest = all_info.substr((all_info.find("Destination") + 13), 1);

			next_hop = all_info.substr(all_info.find("hop:") + 5, 1);

			string end = all_info.substr((all_info.find("Distance") + 9));
			string dist_num = end.substr(0, (end.length() - 1));
			new_distance = atoi(dist_num.c_str());
		}

		catch (const std::out_of_range& oor){	
			//cout <<"catch exit, line was " << r.new_lines[i] << endl;
			return;
		}

		//all_info.erase(0, all_info.find(delim4) + delim4.length() + 1);
		//convert update node and destination node information has been received
		//about into indexes for accessing array     update = node information came from	dest = node information about
		index_update = (int)update_from[0] - 65;
		index_dest = (int)dest[0] - 65;
int now = time(0);
		if(r.router_time[index_dest] == -1 && new_distance < 9999){
				

			r.router_time[index_dest] = now;
		}


		if(index_update >= MAX_SIZE - new_routers){
			//if the router receives an update from a router that is greater than "MAX_SIZE - new_routers"
			//it means that the router it is receiving from is outside of its array window and so that router is new
			//in this case, the new_routers global variable is decremented. This global variable controls the sending and
			//receiving loops throughout the function
			new_routers--;
			if(dest == r.name){
				printToFile(r.new_lines,r.name,ref(r), true);
				r.neighbours[index_update].connected = true;
				r.forwarding.distance[index_update] = new_distance;
				r.forwarding.next[index_update] = update_from;
				printTable(r.forwarding.distance, r.name.at(0), r.forwarding.next);
				printToFile(r.new_lines,r.name,ref(r), false);
			}
		}
	
		if(index_dest >= MAX_SIZE - new_routers){
			//this is similar to the loop above, but it is for the case that this router receives an update from a known
			//router about a router that it does know
			new_routers--;
			printToFile(r.new_lines,r.name,ref(r), true);
			r.forwarding.distance[index_dest] = r.forwarding.distance[index_update] + new_distance;
			r.forwarding.next[index_dest] = r.forwarding.destination[index_update];
			printTable(r.forwarding.distance, r.name.at(0), r.forwarding.next);
			printToFile(r.new_lines,r.name,ref(r), false);
		}

		//---------------------------------------------
		//this quickly reinitialises the forwarding table with all the distances to its own neighbours
		//this allows us to avoid having issues if all the traffic from one router was getting forwarded through the same router
		// (e.g. all traffic from A gets routed through E. if E dies, then this check here will ensure that the table can be rebuilt)
		if(r.forwarding.next[index_dest] == DEAD_CHAR && r.neighbours[index_dest].connected && r.router_time[index_dest] != -1){
			printToFile(r.new_lines,r.name,ref(r), true);
			r.forwarding.distance[index_dest] = r.neighbour_costs[index_dest];
			r.forwarding.next[index_dest] = r.neighbours[index_dest].name;
			printTable(r.forwarding.distance, r.name.at(0), r.forwarding.next);
			printToFile(r.new_lines,r.name,ref(r), false);

		}
		//-------------------------------------------------
	

		if ( (r.forwarding.distance[index_dest] > new_distance + r.forwarding.distance[index_update]) && (r.router_time[index_dest] != -1)) {
			printToFile(r.new_lines,r.name,ref(r), true);
			r.forwarding.distance[index_dest] = new_distance + r.forwarding.distance[index_update];
			r.forwarding.next[index_dest] = r.forwarding.next[index_update];
			printTable(r.forwarding.distance, r.name.at(0), r.forwarding.next);
			printToFile(r.new_lines,r.name,ref(r), false);
		}
		i++;
	}
}

//forwardData updated 4/4/19
void forwardData(router &r, string line) {
	//need to forward data onto new table here

	string source;
	string dest;

	string data;
    string source_name;
    string arrival_name;
    string length;
    time_t _tm = time(NULL);
    struct tm * curtime = localtime(&_tm);
    string time = asctime(curtime);
	int end_port, source_port, arrival_port;

	// if the message being sent is "close" then the router will shut down
	data = line.substr(line.find("data: ") + string("data: ").length());
	if(strcmp(data.c_str()," !close!") == 0){
		cout << "\n\nCLOSE\n\n";
		abort();
	}

	source = line.substr(line.find("src: ") + string("src: ").length(), 1);
    dest = line.substr(line.find("dest: ") + string("dest: ").length(), 1);


	end_port = ((int)dest[0] + 9935);
	length = to_string(data.length());

    //string forward_data = source + " " + src_port + "\n" + dest + " " + dest_port + "\n" + length;
	string forward_data = "Packet Size: " + length + "\n" + time;

	if(dest == r.name){
		source =line.substr(line.find("src: ") + string("src: ").length(), 1);
		//if this is the destination
		cout << "recieved message from " << source << "\n message contents:\n" << endl;
		cout << line.substr(line.find("data: ") + string("data: ").length()) << endl;
        forward_data = forward_data + data;
		source_port = ((int)r.name[0] + 9935);
		source_name = r.name;
		arrival_port = end_port;
		arrival_name = dest;
	}
	else{
		//erase the parts of the message with source and dest info
		source_port = ((int)r.name[0] + 9935);
		source_name = r.name;
		arrival_port = r.send_forward(dest, line);
		arrival_name = (char)(arrival_port - 9935);
		cout << "forwarding to " << dest << endl;
	}
	printToFile_Forwarding(forward_data, arrival_port, source_port, arrival_name, source_name);
}

//-------------------------------------------------

void remakeTable(router &r){
	//get router to throw away its current best paths, since some of them will have been computed using 
	//the dead router
	for(int i = 0; i < MAX_SIZE - new_routers; i ++){
		//if the index corresponds to a dead router, or to the router itself, then just continue
		if(i == r.name.at(0) - 65 || r.router_time[i] == -1)continue;
		if(r.neighbours[i].connected){
			r.forwarding.distance[i] = r.neighbour_costs[i];
			r.forwarding.next[i] = r.neighbours[i].name;
		}else{			
			r.forwarding.distance[i] = 9999;
			r.forwarding.next[i] = DEAD_CHAR;	
		}
	}
}

//-------------------------------------------------

//parseNewLines updated 4/4/19
void parseNewLines(string lines[], string src_name, router &r) {

	int sending_control = -1;

	for (int i = 0; i < 2 * (MAX_SIZE); i++) {
		if (!lines[i].length()) {
			break;
		}
		if (lines[i].find(CONTROL_STRING) != string::npos) {
			sending_control = 1;
		}
		else if (lines[i].find(DATA_STRING) != string::npos) {
			sending_control = 0;
			forwardData(ref(r), lines[i]);
		}
		else if(lines[i].find(DEAD_STRING) != string::npos){
			//DEAD C	<- means C is a dead router
			char dead_router_name = lines[i].substr(lines[i].find(DEAD_STRING)+strlen(DEAD_STRING) +1, 1).at(0);
			cout << "dead router name =" << dead_router_name<< endl; 
			int router_index = dead_router_name - 65;
			remakeTable(ref(r));
			r.handleDeadRouters(router_index);	//r.router_time[router_index] = -1;
			
		}else {
			if (sending_control == 1)update(r); 
		}
	}
}

void receive_th(router &r, struct sockaddr_in re_addr) {
	cout << "recv thread started " << endl;
	char buf[255] = { 0 };
	socklen_t length = sizeof(re_addr);

	string buffer;

	//-------------------------------------

	int curr_line = 0;

	for (int i = 0; i < 2 * (MAX_SIZE - new_routers); i++) {
		r.new_lines[i] = "";
	}
//---------------------------------------------------
retry_recv:
	if (recvfrom(r.sockfd, buf, 255, 0, (struct sockaddr*) &re_addr, &length) != -1) {
		string st(buf);
		//cout << buf; 
			if ((st.find(CONTROL_STRING) != string::npos || st.find(DATA_STRING) != string::npos) && r.new_lines[0] != "") {
			//this means that we need had data in the new_lines, and now we need to send that off
			//those old lines to get parsed and dealt with, and then continue listening

			parseNewLines(r.new_lines, r.name, ref(r));
			//thread print_to_file(printToFile, r.new_lines, r.name, ref(r));
			//print_to_file.detach();
			for (int i = 0; i < 2 * (MAX_SIZE - new_routers); i++) {
				if (r.new_lines[i] == "")break;
				r.new_lines[i] = "";
			}
			curr_line = 0;
		}
		r.new_lines[curr_line++] = st;
		memset(buf, 0, strlen(buf));
	}
	goto retry_recv;
}

void send_th(router &r) {
	while (1) {
		this_thread::sleep_for(chrono::seconds(SEND_DELAY));
		r.send_update((char)0x0);
		r.check_router_count((char)0x0, true);

	}
}
