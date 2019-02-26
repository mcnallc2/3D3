#include "HttpRequest.h"

HttpRequest::HttpRequest(string method, string uri, string version, string header, string body){
    Method = method;
    URI = uri;
    Version = version;
    Header = header;
    Body = body;
}

void HttpRequest::setMethod(string method){
	Method = method;
}
string HttpRequest::getMethod() {
	return Method;
}

void HttpRequest::setURI(string uri){
	URI = uri;
}
string HttpRequest::getURI() {
	return URI;
}

void HttpRequest::setVersion(string version){
	Version = version;
}
string HttpRequest::getVersion() {
	return Version;
}

void HttpRequest::setHeader(string header){
	Header = header;
}
string HttpRequest::getHeader() {
	return Header;
}

void HttpRequest::setBody(string body){
	Body = body;
}
string HttpRequest::getBody() {
	return Body;
}