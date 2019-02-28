#include "HttpResponse.hpp"

HttpResponse::HttpResponse(string version, string status, string header, string html_file){
    Version = version;
    Status_Code = status;
    Header = header;
	HTML = html_file;
}

string HttpResponse::getVersion() {
	return Version;
}

string HttpResponse::getStatus() {
	return Status_Code;
}

string HttpResponse::getHeader() {
	return Header;
}

string HttpResponse::getHTML() {
	return HTML;
}