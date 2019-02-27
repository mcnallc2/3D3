#include "HttpRequest.hpp"

HttpRequest::HttpRequest(string method, string uri, string version, string header, string body){
    Method = method;
    URI = uri;
    Version = version;
    Header = header;
    Body = body;
}

string HttpRequest::getMethod() {
	return Method;
}

string HttpRequest::getURI() {
	return URI;
}

string HttpRequest::getVersion() {
	return Version;
}

string HttpRequest::getHeader() {
	return Header;
}

string HttpRequest::getBody() {
	return Body;
}

// void HttpRequest::setMethod(string method){
// 	Method = method;
// }
// void HttpRequest::setURI(string uri){
// 	URI = uri;
// }
// void HttpRequest::setVersion(string version){
// 	Version = version;
// }
// void HttpRequest::setHeader(string header){
// 	Header = header;
// }
// void HttpRequest::setBody(string body){
// 	Body = body;
// }