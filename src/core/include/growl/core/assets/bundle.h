#pragma once

#include "atlas.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "nlohmann/json.hpp"
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using nlohmann::json;

namespace Growl {

class FontFace;
class Image;
class SystemAPI;
class ShaderPack;
enum class ShaderType;

typedef uint64_t AssetsBundleVersion;

constexpr AssetsBundleVersion ASSETS_BUNDLE_LATEST_VERSION = 1;

enum class AssetType : uint64_t {
	Unknown,
	Image,
	Atlas,
	Font,
	Audio,
	Text,
	ShaderPack
};

std::string getAssetTypeName(AssetType type);

struct AssetsBundleMapInfo {
	uint64_t position;
	uint64_t size;
};

struct AssetsBundleMSDFFontInfo {
	uint64_t msdf_position;
	uint64_t msdf_size;
	std::unordered_map<int, AtlasRegion> glyphs;
};

struct AssetsBundleShaderSourceInfo {
	uint64_t vertex_pos;
	uint64_t vertex_size;
	uint64_t uniforms_pos;
	uint64_t uniforms_size;
	uint64_t fragment_pos;
	uint64_t fragment_size;
};

struct AssetsBundleShaderPackInfo {
	std::string name;
	std::unordered_map<ShaderType, AssetsBundleShaderSourceInfo> sources;
};

struct AssetInfo {
	uint64_t position;
	uint64_t size;
	AssetType type;
	std::optional<std::unordered_map<std::string, AtlasRegion>> atlas_regions;
	std::optional<AssetsBundleMSDFFontInfo> font;
	std::optional<AssetsBundleShaderPackInfo> shader_pack;
};

// Turn off name linting as json library needs these.
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetsBundleMSDFFontInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetsBundleMSDFFontInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetsBundleShaderPackInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetsBundleShaderPackInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetsBundleShaderSourceInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetsBundleShaderSourceInfo& r);

using AssetsMap = std::map<std::string, AssetInfo>;

class AssetsBundle {
public:
	explicit AssetsBundle(
		std::unique_ptr<File> file, std::string path,
		AssetsMap& assets_map) noexcept
		: file{std::move(file)}
		, path{path}
		, assetsMap{std::move(assets_map)} {}

	AssetsMap& getAssetsMap() {
		return assetsMap;
	}

	Result<Image> getImage(std::string name) noexcept;
	Result<Atlas> getAtlas(std::string name) noexcept;
	Result<FontFace> getBitmapFont(
		std::string name, int size, std::string characters = "") noexcept;
	Result<FontFace> getDistanceFieldFont(std::string name) noexcept;
	Result<ShaderPack> getShaderPack(std::string name) noexcept;
	Result<std::string> getTextFileAsString(std::string name) noexcept;
	Result<std::vector<unsigned char>> getRawData(std::string name) noexcept;

	// Returns a File pointer to a specific asset.
	Result<std::unique_ptr<File>>
	getAssetAsFile(SystemAPI& system, std::string name) noexcept;

private:
	std::unique_ptr<File> file;
	std::string path;
	AssetsMap assetsMap;
};

Result<AssetsBundle>
loadAssetsBundle(SystemAPI& system, std::string file_path) noexcept;
Result<AssetsBundle>
loadAssetsBundle(std::unique_ptr<File> file, std::string file_path) noexcept;

} // namespace Growl
