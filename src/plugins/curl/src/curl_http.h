#pragma once

#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include <curl/curl.h>
#include <unordered_map>

namespace Growl {

class CurlHttpRequest : public HttpRequest {
public:
	explicit CurlHttpRequest(CURL* request, curl_slist* headers)
		: request{request}
		, headers{headers} {}
	~CurlHttpRequest();

	CURL* getRequest() {
		return this->request;
	}

private:
	CURL* request;
	curl_slist* headers;
};

class CurlHttpRequestBuilder : public HttpRequestBuilder {
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
