#pragma once

#include <cstdint>
#include <string>

namespace Growl {
class UUID {
public:
	explicit UUID();
	std::string toString();

private:
	std::uint64_t a;
	std::uint64_t b;
};
} // namespace Growl
