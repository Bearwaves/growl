#pragma once

#include "growl/core/error.h"
#include <Foundation/Foundation.h>
namespace Growl {

class MetalError : public BaseError {
public:
	explicit MetalError(std::string message)
		: message_str{message} {}

	explicit MetalError(NSError* ns_err)
		: message_str{[[ns_err localizedDescription] UTF8String]} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
