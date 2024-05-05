#include "growl/core/assets/shader_pack.h"
#include "../thirdparty/rang.hpp"
#include "assets_config.h"
#include "error.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/error.h"
#include <filesystem>
#include <iostream>
#include <unordered_map>

using Growl::AssetInfo;
using Growl::AssetsBundleShaderPackInfo;
using Growl::AssetsBundleShaderSourceInfo;
using Growl::AssetsIncludeError;
using Growl::AssetsIncludeErrorCode;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::Result;
using Growl::ShaderPackConfig;
using Growl::ShaderSource;
using Growl::ShaderType;
using rang::style;

Result<std::string> readSource(const std::filesystem::directory_entry& entry) {
	std::ifstream file;
	file.open(entry.path(), std::ios::in);
	if (file.fail()) {
		return Growl::Error(std::make_unique<AssetsIncludeError>(
			"Failed to open file " + entry.path().generic_string()));
	}
	std::ostringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

std::string shaderFormat(ShaderType type) {
	switch (type) {
	case Growl::ShaderType::GLSL:
		return "GLSL";
	case Growl::ShaderType::Metal:
		return "Metal";
	}
	return "Unknown";
}

AssetsIncludeError includeShaderPack(
	const ShaderPackConfig& config, const std::filesystem::path& path,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	std::unordered_map<ShaderType, ShaderSource> sources;
	for (auto& entry : std::filesystem::directory_iterator(path)) {
		ShaderType shader_type;
		std::string ext = entry.path().extension().string();
		if (ext == ".glsl") {
			shader_type = ShaderType::GLSL;
		} else if (ext == ".metal") {
			shader_type = ShaderType::Metal;
		} else {
			continue;
		}

		if (sources.find(shader_type) == sources.end()) {
			sources[shader_type] = ShaderSource{};
		}

		auto source_res = readSource(entry);
		if (!source_res) {
			return AssetsIncludeError(source_res.error()->message());
		}
		std::string source = *source_res;

		if (entry.path().stem().string() == "fragment") {
			sources[shader_type].fragment_src = source;
		} else if (entry.path().stem().string() == "vertex") {
			sources[shader_type].vertex_src = source;
		} else {
			return AssetsIncludeError(
				"Unsupported file name " + entry.path().filename().string() +
				", must be vertex or fragment");
		}

		std::cout << "=> [" << style::bold << resolved_path.string()
				  << style::reset << "] Included " << style::bold
				  << shaderFormat(shader_type) << " "
				  << entry.path().stem().string() << style::reset << " source."
				  << std::endl;
	}

	auto start = static_cast<uint64_t>(outfile.tellp());
	AssetInfo info{start, 0, AssetType::ShaderPack};
	AssetsBundleShaderPackInfo pack_info;
	pack_info.name = config.name;
	for (auto& entry : sources) {
		if (!entry.second.fragment_src.has_value()) {
			return AssetsIncludeError(
				shaderFormat(entry.first) +
				" shader does not include fragment source, which is required.");
		}

		AssetsBundleShaderSourceInfo source_info{0, 0, 0, 0};
		if (entry.second.vertex_src.has_value()) {
			source_info.vertex_pos = outfile.tellp();
			outfile << entry.second.vertex_src.value().c_str();
			source_info.vertex_size =
				static_cast<uint64_t>(outfile.tellp()) - source_info.vertex_pos;
		}
		source_info.fragment_pos = outfile.tellp();
		outfile << entry.second.fragment_src.value().c_str();
		source_info.fragment_size =
			static_cast<uint64_t>(outfile.tellp()) - source_info.fragment_pos;
		pack_info.sources[entry.first] = source_info;
	}
	info.shader_pack = pack_info;
	info.size = static_cast<uint64_t>(outfile.tellp()) - start;

	assets_map[resolved_path.string()] = info;

	return AssetsIncludeErrorCode::None;
}
