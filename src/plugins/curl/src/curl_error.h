#pragma once

#include "growl/core/error.h"
#include <curl/curl.h>

namespace Growl {

class CurlError : public BaseError {
public:
	explicit CurlError(std::string message)
		: message_str{message} {}

	CurlError(std::string message, CURLcode curl_code)
		: message_str{
			  message + ": " + std::string(curl_easy_strerror(curl_code))} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
