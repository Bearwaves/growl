#pragma once

#include "growl/core/error.h"
namespace Growl {

class AndroidError : public BaseError {
public:
	explicit AndroidError(std::string message)
		: message_str{message} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
