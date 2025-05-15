#pragma once

#include "growl/core/error.h"
#include <string>

namespace Growl {

enum class HttpMethod { GET, POST, PUT, PATCH, OPTIONS, HEAD, CONNECT, TRACE };

std::string httpMethodString(HttpMethod method);

class HttpRequest {};

class HttpResponse {
public:
	int status_code = 0;
	std::string content = "";
};

class HttpRequestBuilder {
public:
	virtual ~HttpRequestBuilder() {}
	virtual HttpRequestBuilder& setURL(std::string url) = 0;
	virtual HttpRequestBuilder& setMethod(HttpMethod method) = 0;
	virtual HttpRequestBuilder&
	setHeader(std::string header, std::string value) = 0;
	virtual Result<std::unique_ptr<HttpRequest>> build() = 0;
};

} // namespace Growl
