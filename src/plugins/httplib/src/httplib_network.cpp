#include "httplib_network.h"
#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/network/future.h"
#include "httplib.h"
#include "httplib_error.h"
#include "httplib_http.h"
#include <cstddef>
#include <future>
#include <openssl/evp.h>
#include <openssl/hmac.h>

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
	return std::make_unique<HttplibNetworkAPI>(api);
}

Error HttplibNetworkAPI::init(const Config& config) {
	api.system().log("HttplibNetworkAPI", "Initialised httplib network API");
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

std::vector<unsigned char>
HttplibNetworkAPI::hmac256(std::string& body, std::string& key) {
	unsigned char buf[EVP_MAX_MD_SIZE];
	unsigned int size;
	if (!HMAC(
			EVP_sha256(), key.data(), key.size(),
			reinterpret_cast<unsigned char*>(body.data()), body.size(), buf,
			&size)) {
		return std::vector<unsigned char>();
	}
	return std::vector<unsigned char>(buf, buf + size);
}

std::string HttplibNetworkAPI::base64enc(const unsigned char* data, int len) {
	int b64_len = 4 * ((len + 2) / 3);
	unsigned char* b64_data = (unsigned char*)calloc(b64_len + 1, 1);
	EVP_EncodeBlock(b64_data, data, len);
	auto res = std::string(reinterpret_cast<char*>(b64_data));
	free(b64_data);
	return res;
}
