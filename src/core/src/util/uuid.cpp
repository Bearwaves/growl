#include "growl/core/util/uuid.h"
#include "fmt/format.h"
#include <random>
#include <string>

using Growl::UUID;

constexpr std::uint64_t MASK_A = 0xFFFFFFFFFFFF0FFFull;
constexpr std::uint64_t MASK_B = 0x3FFFFFFFFFFFFFFFull;
constexpr std::uint64_t VERSION = 0x0000000000004000ull;
constexpr std::uint64_t VARIANT = 0x8000000000000000ull;

UUID::UUID() {
	auto rand = std::mt19937_64(std::random_device()());
	a = (rand() & MASK_A) | VERSION;
	b = (rand() & MASK_B) | VARIANT;
}

std::string UUID::toString() {
	return fmt::format(
		"{:08x}-{:04x}-{:04x}-{:04x}-{:012x}", a >> 32, (a >> 16) & 0xFFFF,
		a & 0xFFFF, b >> 48, b & 0xFFFFFFFFFFFFull);
}
