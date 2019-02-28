#include "HttpResponse.hpp"

HttpResponse::HttpResponse(string version, int status, string reason, string header){
    Version = version;
    Status_Code = status;
    Reason = reason;
    Header = header;
}

string HttpResponse::getVersion() {
	return Version;
}

int HttpResponse::getStatus() {
	return Status_Code;
}

string HttpResponse::getReason() {
	return Reason;
}

string HttpResponse::getHeader() {
	return Header;
}

string HttpResponse::getHTML() {
	return HTML;
}

// void HttpResponse::setMethod(string method){
// 	Method = method;
// }
// void HttpResponse::setURI(string uri){
// 	URI = uri;
// }
// void HttpResponse::setVersion(string version){
// 	Version = version;
// }
// void HttpResponse::setHeader(string header){
// 	Header = header;
// }
// void HttpResponse::setBody(string body){
// 	Body = body;
// }