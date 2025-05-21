#include "web_http.h"
#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include "web_error.h"

using Growl::Error;
using Growl::HttpRequest;
using Growl::HttpRequestBuilder;
using Growl::Result;
using Growl::WebError;
using Growl::WebHttpRequest;
using Growl::WebHttpRequestBuilder;

WebHttpRequest::~WebHttpRequest() {}

HttpRequestBuilder& WebHttpRequestBuilder::setURL(std::string url) {
	this->url = url;
	return *this;
}

HttpRequestBuilder& WebHttpRequestBuilder::setMethod(HttpMethod method) {
	this->method = method;
	return *this;
}

HttpRequestBuilder& WebHttpRequestBuilder::setBody(std::string& body) {
	this->body = body;
	return *this;
}

HttpRequestBuilder&
WebHttpRequestBuilder::setHeader(std::string header, std::string value) {
	this->headers[header] = value;
	return *this;
}

Result<std::unique_ptr<HttpRequest>> WebHttpRequestBuilder::build() {
	return Error(std::make_unique<WebError>("Not implemented"));
}
