#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class HttpResponse {

private:

	string Version;
	string Status_Code;
	string Header;
    string HTML;

public:

	HttpResponse(string version, string status, string header, string html_file);	

	string getVersion();
	string getStatus();
	string getHeader();
    string getHTML();

};