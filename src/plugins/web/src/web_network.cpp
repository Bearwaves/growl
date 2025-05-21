#include "web_network.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include "web_error.h"
#include "web_http.h"

using Growl::API;
using Growl::Config;
using Growl::Error;
using Growl::Future;
using Growl::HttpRequestBuilder;
using Growl::HttpResponse;
using Growl::NetworkAPIInternal;
using Growl::Result;
using Growl::WebError;
using Growl::WebNetworkAPI;

std::unique_ptr<NetworkAPIInternal>
Growl::createNetworkAPI(API& api, void* user) {
	return std::make_unique<WebNetworkAPI>(api);
}

Error WebNetworkAPI::init(const Config& config) {
	api.system().log("WebNetworkAPI", "Initialised Web networking");
	return initCrypto();
}

void WebNetworkAPI::dispose() {}

std::unique_ptr<HttpRequestBuilder> WebNetworkAPI::httpRequestBuilder() {
	return std::make_unique<WebHttpRequestBuilder>();
}

Future<HttpResponse>
WebNetworkAPI::doHttpRequest(std::unique_ptr<HttpRequest> request) {
	return std::async(
		std::launch::async,
		[req = std::move(request)]() -> Result<HttpResponse> {
			return Error(std::make_unique<WebError>("Not implemented"));
		});
}
