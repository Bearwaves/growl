#include "curl_http.h"
#include "curl_error.h"
#include "growl/core/error.h"
#include "growl/core/network/http.h"

using Growl::CurlError;
using Growl::CurlHttpRequest;
using Growl::CurlHttpRequestBuilder;
using Growl::Error;
using Growl::HttpRequest;
using Growl::HttpRequestBuilder;
using Growl::Result;

CurlHttpRequest::~CurlHttpRequest() {
	if (this->headers) {
		curl_slist_free_all(headers);
		headers = nullptr;
	}
	if (this->request) {
		curl_easy_cleanup(this->request);
		this->request = nullptr;
	}
}

HttpRequestBuilder& CurlHttpRequestBuilder::setURL(std::string url) {
	this->url = url;
	return *this;
}

HttpRequestBuilder& CurlHttpRequestBuilder::setMethod(HttpMethod method) {
	this->method = method;
	return *this;
}

HttpRequestBuilder& CurlHttpRequestBuilder::setBody(std::string& body) {
	this->body = body;
	return *this;
}

HttpRequestBuilder&
CurlHttpRequestBuilder::setHeader(std::string header, std::string value) {
	this->headers[header] = value;
	return *this;
}

Result<std::unique_ptr<HttpRequest>> CurlHttpRequestBuilder::build() {
	CURL* curl = curl_easy_init();
	if (!curl) {
		return Error(
			std::make_unique<CurlError>("Failed to create CURL request"));
	}
	if (auto err = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		err != CURLE_OK) {
		return Error(
			std::make_unique<CurlError>("Failed to set CURL URL", err));
	}
	if (auto err = curl_easy_setopt(
			curl, CURLOPT_CUSTOMREQUEST, httpMethodString(method).c_str());
		err != CURLE_OK) {
		return Error(
			std::make_unique<CurlError>("Failed to set CURL method", err));
	}
	if (auto err = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		err != CURLE_OK) {
		return Error(
			std::make_unique<CurlError>("Failed to set CURL follow", err));
	}

	if (!body.empty()) {
		if (auto err =
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
			err != CURLE_OK) {
			return Error(
				std::make_unique<CurlError>(
					"Failed to set CURL body size", err));
		}
		if (auto err =
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.data());
			err != CURLE_OK) {
			return Error(
				std::make_unique<CurlError>("Failed to set CURL body", err));
		}
	}

	curl_slist* headers_list = nullptr;
	for (auto& [k, v] : headers) {
		headers_list =
			curl_slist_append(headers_list, std::string(k + ": " + v).c_str());
	}
	if (auto err = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);
		err != CURLE_OK) {
		return Error(
			std::make_unique<CurlError>("Failed to set CURL headers", err));
	}

	return std::unique_ptr<HttpRequest>(
		std::make_unique<CurlHttpRequest>(curl, headers_list));
}
