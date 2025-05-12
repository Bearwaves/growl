#include "ios_http.h"
#include "growl/core/error.h"
#include "ios_error.h"

using Growl::Error;
using Growl::HttpRequest;
using Growl::HttpRequestBuilder;
using Growl::IOSError;
using Growl::IOSHttpRequest;
using Growl::IOSHttpRequestBuilder;
using Growl::Result;

IOSHttpRequest::IOSHttpRequest(NSMutableURLRequest* request)
	: request{[request retain]} {}

IOSHttpRequest::~IOSHttpRequest() {
	if (request) {
		[request release];
	}
}

HttpRequestBuilder& IOSHttpRequestBuilder::setURL(std::string url) {
	this->url = url;
	return *this;
}

HttpRequestBuilder& IOSHttpRequestBuilder::setMethod(HttpMethod method) {
	this->method = method;
	return *this;
}

HttpRequestBuilder&
IOSHttpRequestBuilder::setHeader(std::string header, std::string value) {
	this->headers[header] = value;
	return *this;
}

Result<std::unique_ptr<HttpRequest>> IOSHttpRequestBuilder::build() {
	NSURL* url =
		[NSURL URLWithString:[NSString stringWithUTF8String:this->url.c_str()]];
	if (!url) {
		return Error(
			std::make_unique<IOSError>("Failed to parse URL " + this->url));
	}
	auto request = [NSMutableURLRequest requestWithURL:url];
	request.HTTPMethod =
		[NSString stringWithUTF8String:httpMethodString(this->method).c_str()];
	for (auto& [k, v] : this->headers) {
		[request setValue:[NSString stringWithUTF8String:v.c_str()]
			forHTTPHeaderField:[NSString stringWithUTF8String:k.c_str()]];
	}
	return std::unique_ptr<HttpRequest>(
		std::make_unique<IOSHttpRequest>(request));
}
