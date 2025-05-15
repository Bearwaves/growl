#pragma once

#include "growl/core/network/future.h"
#include "growl/core/network/http.h"
#include <memory>

namespace Growl {

class NetworkAPI {
public:
	virtual ~NetworkAPI() {}

	virtual std::unique_ptr<HttpRequestBuilder> httpRequestBuilder() = 0;
	virtual Future<HttpResponse>
	doHttpRequest(std::unique_ptr<HttpRequest> request) = 0;
};

} // namespace Growl
