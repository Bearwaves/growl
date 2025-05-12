#pragma once

#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include <Foundation/Foundation.h>
#include <unordered_map>

namespace Growl {

class IOSHttpRequest : public HttpRequest {
public:
	explicit IOSHttpRequest(NSMutableURLRequest* request);
	~IOSHttpRequest();
	NSMutableURLRequest* getNative() {
		return request;
	}

private:
	NSMutableURLRequest* request;
};

class IOSHttpRequestBuilder : public HttpRequestBuilder {
public:
	HttpRequestBuilder& setURL(std::string url) override;
	HttpRequestBuilder& setMethod(HttpMethod method) override;
	HttpRequestBuilder&
	setHeader(std::string header, std::string value) override;
	Result<std::unique_ptr<HttpRequest>> build() override;

private:
	HttpMethod method = HttpMethod::GET;
	std::string url = "";
	std::unordered_map<std::string, std::string> headers;
};

} // namespace Growl
