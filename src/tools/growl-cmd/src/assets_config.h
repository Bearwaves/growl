#pragma once

#include "nlohmann/json.hpp"
#include <optional>
#include <string>

using nlohmann::json;

namespace Growl {

struct AtlasConfig {
	int padding = 4;
};

struct FontConfig {
	bool msdf;
	int msdfSize = 32;
	std::string msdfCharacters = "";
};

struct AssetConfig {
	std::optional<AtlasConfig> atlas;
	std::optional<FontConfig> font;
};

// Turn off name linting as json library needs these.
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetConfig& s);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AtlasConfig& s);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, FontConfig& s);
} // namespace Growl
