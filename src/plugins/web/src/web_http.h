#pragma once

#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include <unordered_map>

namespace Growl {

class WebHttpRequest : public HttpRequest {
public:
	explicit WebHttpRequest() {}
	~WebHttpRequest();

private:
};

class WebHttpRequestBuilder : public HttpRequestBuilder {
public:
	HttpRequestBuilder& setURL(std::string url) override;

	HttpRequestBuilder& setMethod(HttpMethod method) override;

	HttpRequestBuilder& setBody(std::string& body) override;

	HttpRequestBuilder&
	setHeader(std::string header, std::string value) override;

	Result<std::unique_ptr<HttpRequest>> build() override;

private:
	HttpMethod method = HttpMethod::GET;
	std::string url = "";
	std::string body = "";
	std::unordered_map<std::string, std::string> headers;
};

} // namespace Growl
