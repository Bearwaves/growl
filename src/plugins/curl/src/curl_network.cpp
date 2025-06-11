#include "curl_network.h"
#include "curl_error.h"
#include "curl_http.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include <atomic>

using Growl::API;
using Growl::Config;
using Growl::CurlError;
using Growl::CurlNetworkAPI;
using Growl::Error;
using Growl::Future;
using Growl::HttpRequestBuilder;
using Growl::HttpResponse;
using Growl::NetworkAPIInternal;
using Growl::Result;

std::unique_ptr<NetworkAPIInternal>
Growl::createNetworkAPI(API& api, void* user) {
	return std::make_unique<CurlNetworkAPI>(api);
}

Error CurlNetworkAPI::init(const Config& config) {
	auto err = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (err != CURLE_OK) {
		return std::make_unique<CurlError>(
			"Failed to init curl, got error", err);
	}
	api.system().log("CurlNetworkAPI", "Initialised Curl networking");
	return nullptr;
}

void CurlNetworkAPI::dispose() {
	curl_global_cleanup();
}

std::unique_ptr<HttpRequestBuilder> CurlNetworkAPI::httpRequestBuilder() {
	return std::make_unique<CurlHttpRequestBuilder>();
}

size_t writeToString(void* contents, size_t size, size_t n, void* user) {
	size_t real_size = size * n;
	std::string* body = static_cast<std::string*>(user);
	body->append(static_cast<char*>(contents), real_size);
	return real_size;
}

int progressCallback(
	void* clientp, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
	auto cancellation_token = static_cast<std::atomic_bool*>(clientp);
	return cancellation_token->load() ? 1 : 0;
}

Future<HttpResponse>
CurlNetworkAPI::doHttpRequest(std::unique_ptr<HttpRequest> request) {
	auto cancellation_token = std::make_shared<std::atomic_bool>(false);
	return Future(
		std::async(
			std::launch::async,
			[req = std::move(request),
			 cancellation_token]() -> Result<HttpResponse> {
				CURL* c =
					static_cast<CurlHttpRequest*>(req.get())->getRequest();
				HttpResponse response;

				if (auto err = curl_easy_setopt(
						c, CURLOPT_WRITEFUNCTION, writeToString);
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to set response write function", err));
				}
				if (auto err = curl_easy_setopt(
						c, CURLOPT_WRITEDATA, &(response.content));
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to set response write pointer", err));
				}

				if (auto err = curl_easy_setopt(c, CURLOPT_NOPROGRESS, 0L);
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to set noprogress", err));
				}

				if (auto err = curl_easy_setopt(
						c, CURLOPT_XFERINFODATA, cancellation_token.get());
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to set progress data", err));
				}

				if (auto err = curl_easy_setopt(
						c, CURLOPT_XFERINFOFUNCTION, progressCallback);
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to set progress data", err));
				}

				if (auto err = curl_easy_perform(c); err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to make request", err));
				}
				if (auto err = curl_easy_getinfo(
						c, CURLINFO_RESPONSE_CODE, &(response.status_code));
					err != CURLE_OK) {
					return Error(
						std::make_unique<CurlError>(
							"Failed to read response status code", err));
				}
				return response;
			}),
		std::move(cancellation_token));
}
