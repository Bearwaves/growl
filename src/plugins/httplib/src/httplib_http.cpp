#include "httplib_http.h"
#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include "httplib.h"
#include "httplib_error.h"

using Growl::Error;
using Growl::HttplibError;
using Growl::HttplibHttpRequest;
using Growl::HttplibHttpRequestBuilder;
using Growl::HttpRequest;
using Growl::HttpRequestBuilder;
using Growl::Result;

constexpr const char* PROTOCOL_DIVIDER = "://";

HttpRequestBuilder& HttplibHttpRequestBuilder::setURL(std::string url) {
	this->url = url;
	return *this;
}

HttpRequestBuilder& HttplibHttpRequestBuilder::setMethod(HttpMethod method) {
	this->method = method;
	return *this;
}

HttpRequestBuilder&
HttplibHttpRequestBuilder::setHeader(std::string header, std::string value) {
	this->headers[header] = value;
	return *this;
}

Result<std::unique_ptr<HttpRequest>> HttplibHttpRequestBuilder::build() {
	httplib::Request request;

	std::string url_base;
	auto protocol_pos = url.find(PROTOCOL_DIVIDER);
	auto path_pos =
		url.find('/', protocol_pos == std::string::npos ? 0 : protocol_pos + 3);
	if (path_pos == std::string::npos) {
		url_base = url;
		request.path = "/";
	} else {
		url_base = url.substr(0, path_pos);
		request.path = url.substr(path_pos);
	}

	request.method = Growl::httpMethodString(method);
	request.headers.insert(headers.begin(), headers.end());
	return std::unique_ptr<HttpRequest>(
		std::make_unique<HttplibHttpRequest>(url_base, std::move(request)));
}
