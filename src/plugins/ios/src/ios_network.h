#pragma once

#include "growl/core/api/api_internal.h"

namespace Growl {

class API;

class IOSNetworkAPI : public NetworkAPIInternal {
public:
	explicit IOSNetworkAPI() {}

	Error init(const Config& config) override;
	void dispose() override;

	std::unique_ptr<HttpRequestBuilder> httpRequestBuilder() override;
	Future<HttpResponse> doHttpRequest(HttpRequest& request) override;
};

} // namespace Growl
