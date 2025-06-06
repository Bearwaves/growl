#pragma once

#include "growl/core/api/api_internal.h"

namespace Growl {

class API;

class CurlNetworkAPI : public NetworkAPIInternal {
public:
	explicit CurlNetworkAPI(API& api)
		: api{api} {}

	Error init(const Config& config) override;
	void dispose() override;

	std::unique_ptr<HttpRequestBuilder> httpRequestBuilder() override;
	Future<HttpResponse>
	doHttpRequest(std::unique_ptr<HttpRequest> request) override;

private:
	API& api;
};

} // namespace Growl
