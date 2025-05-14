#include "httplib_network.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include "httplib.h"
#include "httplib_error.h"
#include "httplib_http.h"
#include <future>

using Growl::API;
using Growl::Config;
using Growl::Error;
using Growl::Future;
using Growl::HttplibError;
using Growl::HttplibHttpRequestBuilder;
using Growl::HttplibNetworkAPI;
using Growl::HttpRequestBuilder;
using Growl::HttpResponse;
using Growl::NetworkAPIInternal;
using Growl::Result;

std::unique_ptr<NetworkAPIInternal>
Growl::createNetworkAPI(API& api, void* user) {
	return std::make_unique<HttplibNetworkAPI>();
}

Error HttplibNetworkAPI::init(const Config& config) {
	return nullptr;
}

void HttplibNetworkAPI::dispose() {}

std::unique_ptr<HttpRequestBuilder> HttplibNetworkAPI::httpRequestBuilder() {
	return std::make_unique<HttplibHttpRequestBuilder>();
}

Future<HttpResponse>
HttplibNetworkAPI::doHttpRequest(std::unique_ptr<HttpRequest> request) {
	return std::async(
		std::launch::async,
		[request = std::move(request)]() -> Result<HttpResponse> {
			auto req = static_cast<HttplibHttpRequest*>(request.get());
			auto cli = httplib::Client(req->getURL());
			auto response = cli.send(req->getRequest());
			if (response.error() != httplib::Error::Success) {
				return Error(std::make_unique<HttplibError>(response.error()));
			}
			HttpResponse resp;
			resp.status_code = response->status;
			resp.content = response->body;
			return resp;
		});
}
