#include "ios_network.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include "ios_error.h"
#include "ios_http.h"
#include <thread>

using Growl::API;
using Growl::Config;
using Growl::Error;
using Growl::Future;
using Growl::HttpRequestBuilder;
using Growl::HttpResponse;
using Growl::IOSError;
using Growl::IOSHttpRequestBuilder;
using Growl::IOSNetworkAPI;
using Growl::NetworkAPIInternal;
using Growl::Result;

std::unique_ptr<NetworkAPIInternal>
Growl::createNetworkAPI(API& api, void* user) {
	return std::make_unique<IOSNetworkAPI>(api);
}

Error IOSNetworkAPI::init(const Config& config) {
	api.system().log("IOSNetworkAPI", "Initialised iOS network API");
	return initCrypto();
}

void IOSNetworkAPI::dispose() {}

std::unique_ptr<HttpRequestBuilder> IOSNetworkAPI::httpRequestBuilder() {
	return std::make_unique<IOSHttpRequestBuilder>();
}

Future<HttpResponse>
IOSNetworkAPI::doHttpRequest(std::unique_ptr<HttpRequest> request) {
	auto ios_request = static_cast<IOSHttpRequest&>(*request);
	__block auto promise = std::promise<Result<HttpResponse>>();
	auto task = [[[NSURLSession sharedSession]
		dataTaskWithRequest:ios_request.getNative()
		  completionHandler:^(
			  NSData* data, NSURLResponse* response, NSError* error) {
			if (error) {
				promise.set_value(Error(std::make_unique<IOSError>(error)));
				return;
			}
			HttpResponse resp;
			resp.content = std::string([[[NSString alloc]
				initWithData:data
					encoding:NSUTF8StringEncoding] UTF8String]);
			if ([response respondsToSelector:@selector(statusCode)]) {
				auto http_response = static_cast<NSHTTPURLResponse*>(response);
				resp.status_code = static_cast<int>([http_response statusCode]);
			}
			promise.set_value(std::move(resp));
		  }] retain];
	[task resume];

	auto cancellation_lambda = [task]() {
		[task cancel];
		[task release];
	};

	return Future(promise.get_future(), std::move(cancellation_lambda));
}
