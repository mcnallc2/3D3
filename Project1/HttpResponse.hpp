#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class HttpResponse {

private:

	string Version;
	int Status_Code;
	string Reason;
	string Header;
    string HTML;

public:

	HttpResponse(string version, int status, string reason, string header);	

	string getVersion();
	int getStatus();
	string getReason();
	string getHeader();
    string getHTML();

};