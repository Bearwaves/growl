#pragma once

#include "growl/core/network/future.h"
#include "growl/core/network/http.h"
#include <memory>
#include <vector>

namespace Growl {

class NetworkAPI {
public:
	virtual ~NetworkAPI() {}

	virtual std::unique_ptr<HttpRequestBuilder> httpRequestBuilder() = 0;
	virtual Future<HttpResponse>
	doHttpRequest(std::unique_ptr<HttpRequest> request) = 0;

	virtual std::vector<unsigned char>
	hmac256(std::string& body, std::string& key);
	virtual std::string base64enc(const unsigned char* data, int len);
	virtual Result<std::string> gzip(std::string& data);
	virtual std::string sha256(std::string& data);
};

} // namespace Growl
