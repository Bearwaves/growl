#pragma once

#include "growl/core/error.h"
#include "growl/core/network/http.h"
#include "httplib.h"
#include <unordered_map>

namespace Growl {

class HttplibHttpRequest : public HttpRequest {
public:
	explicit HttplibHttpRequest(std::string url, httplib::Request&& request)
		: url{url}
		, request{std::move(request)} {}

	std::string& getURL() {
		return url;
	}

	httplib::Request& getRequest() {
		return this->request;
	}

private:
	std::string url;
	httplib::Request request;
};

class HttplibHttpRequestBuilder : public HttpRequestBuilder {
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
