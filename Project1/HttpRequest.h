#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class HttpRequest {

private:

	string Method;
	string URI;
	string Version;
	string Header;
	string Body;


public:

	HttpRequest(string method, string uri, string version, string header, string body);
	void setMethod(string method);
	void setURI(string uri);
	void setVersion(string version);
	void setHeader(string header);
	void setBody(string body);

	string getMethod();
	string getURI();
	string getVersion();
	string getHeader();
	string getBody();

};