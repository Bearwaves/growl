#pragma once

#include "nlohmann/json.hpp"
#include <optional>
#include <string>

using nlohmann::json;

namespace Growl {

struct AtlasConfig {
	int padding = 2;
	int bleed_passes = 2;
	bool recursive = false;
};

struct FontConfig {
	bool msdf = false;
	int msdfSize = 32;
	std::string msdfCharacters = "";
};

struct ShaderPackConfig {
	std::string name = "";
};

struct AssetConfig {
	std::optional<AtlasConfig> atlas;
	std::optional<FontConfig> font;
	std::optional<ShaderPackConfig> shader_pack;
	bool text = false;
};

// Turn off name linting as json library needs these.
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetConfig& s);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AtlasConfig& s);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, FontConfig& s);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, ShaderPackConfig& s);
} // namespace Growl
