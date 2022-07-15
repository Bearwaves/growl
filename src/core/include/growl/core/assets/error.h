#pragma once

#include "../error.h"
namespace Growl {

class AssetsError : public BaseError {
public:
	explicit AssetsError(std::string message)
		: message_str{message} {}
	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
