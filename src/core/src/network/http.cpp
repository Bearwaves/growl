#include "growl/core/network/http.h"

using Growl::HttpMethod;

std::string Growl::httpMethodString(HttpMethod method) {
	switch (method) {
	case HttpMethod::GET:
		return "GET";
	case HttpMethod::POST:
		return "POST";
	case HttpMethod::PUT:
		return "PUT";
	case HttpMethod::PATCH:
		return "PATCH";
	case HttpMethod::OPTIONS:
		return "OPTIONS";
	case HttpMethod::HEAD:
		return "HEAD";
	case HttpMethod::CONNECT:
		return "CONNECT";
	case HttpMethod::TRACE:
		return "TRACE";
	}
	return "";
}
