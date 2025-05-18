#include "android_network.h"
#include "android_error.h"
#include "android_http.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include <future>
#include <openssl/evp.h>
#include <openssl/hmac.h>

using Growl::AndroidError;
using Growl::AndroidNetworkAPI;
using Growl::API;
using Growl::Config;
using Growl::Error;
using Growl::Future;
using Growl::HttpRequestBuilder;
using Growl::HttpResponse;
using Growl::NetworkAPIInternal;
using Growl::Result;

std::unique_ptr<NetworkAPIInternal>
Growl::createNetworkAPI(API& api, void* user) {
	return std::make_unique<AndroidNetworkAPI>(api);
}

Error AndroidNetworkAPI::init(const Config& config) {
	auto err = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (err != CURLE_OK) {
		return std::make_unique<AndroidError>(
			"Failed to init curl, got error", err);
	}
	api.system().log(
		"AndroidNetworkAPI", "Initialised Android [libcurl] networking");
	return nullptr;
}

void AndroidNetworkAPI::dispose() {
	curl_global_cleanup();
}

std::unique_ptr<HttpRequestBuilder> AndroidNetworkAPI::httpRequestBuilder() {
	return std::make_unique<AndroidHttpRequestBuilder>();
}

size_t writeToString(void* contents, size_t size, size_t n, void* user) {
	size_t real_size = size * n;
	std::string* body = static_cast<std::string*>(user);
	body->append(static_cast<char*>(contents), real_size);
	return real_size;
}

Future<HttpResponse>
AndroidNetworkAPI::doHttpRequest(std::unique_ptr<HttpRequest> request) {
	return std::async(
		std::launch::async,
		[req = std::move(request)]() -> Result<HttpResponse> {
			CURL* c = static_cast<AndroidHttpRequest*>(req.get())->getRequest();
			HttpResponse response;
			curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);

			if (auto err =
					curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, writeToString);
				err != CURLE_OK) {
				return Error(
					std::make_unique<AndroidError>(
						"Failed to set response write function", err));
			}
			if (auto err =
					curl_easy_setopt(c, CURLOPT_WRITEDATA, &(response.content));
				err != CURLE_OK) {
				return Error(
					std::make_unique<AndroidError>(
						"Failed to set response write pointer", err));
			}

			if (auto err = curl_easy_perform(c); err != CURLE_OK) {
				return Error(
					std::make_unique<AndroidError>(
						"Failed to make request", err));
			}
			if (auto err = curl_easy_getinfo(
					c, CURLINFO_RESPONSE_CODE, &(response.status_code));
				err != CURLE_OK) {
				return Error(
					std::make_unique<AndroidError>(
						"Failed to read response status code", err));
			}
			return response;
		});
}

std::vector<unsigned char>
AndroidNetworkAPI::hmac256(std::string& body, std::string& key) {
	unsigned char buf[EVP_MAX_MD_SIZE];
	unsigned int size;
	if (!HMAC(
			EVP_sha256(), key.data(), key.size(),
			reinterpret_cast<const unsigned char*>(body.data()), body.size(),
			buf, &size)) {
		return std::vector<unsigned char>();
	}
	return std::vector<unsigned char>(buf, buf + size);
}

std::string AndroidNetworkAPI::base64enc(const unsigned char* data, int len) {
	int b64_len = 4 * ((len + 2) / 3);
	unsigned char* b64_data = (unsigned char*)calloc(b64_len + 1, 1);
	EVP_EncodeBlock(b64_data, data, len);
	auto res = std::string(reinterpret_cast<char*>(b64_data));
	free(b64_data);
	return res;
}
