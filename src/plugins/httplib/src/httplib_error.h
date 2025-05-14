#pragma once

#include "growl/core/error.h"
#include "httplib.h"

namespace Growl {

class HttplibError : public BaseError {
public:
	explicit HttplibError(std::string message)
		: message_str{message} {}

	explicit HttplibError(httplib::Error err)
		: message_str{std::string(
			  "Failed to make request: " + httplib::to_string(err))} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
