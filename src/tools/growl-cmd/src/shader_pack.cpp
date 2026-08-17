#include "growl/core/assets/shader_pack.h"
#include "../thirdparty/rang.hpp"
#include "assets_config.h"
#include "error.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/error.h"
#include "growl/core/error.h"
#include "nlohmann/json.hpp"
#include "shader_defaults.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

using Growl::AssetInfo;
using Growl::AssetsBundleShaderPackInfo;
using Growl::AssetsBundleShaderSourceInfo;
using Growl::AssetsError;
using Growl::AssetsIncludeError;
using Growl::AssetsIncludeErrorCode;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::Error;
using Growl::Result;
using Growl::ShaderDefaults;
using Growl::ShaderPackConfig;
using Growl::ShaderSource;
using Growl::ShaderType;
using rang::style;

Result<std::string> readSource(const std::filesystem::directory_entry& entry) {
	std::ifstream file;
	file.open(entry.path(), std::ios::in);
	if (file.fail()) {
		return Growl::Error(
			std::make_unique<AssetsIncludeError>(
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
		} else if (entry.path().stem().string() == "uniforms") {
			sources[shader_type].uniforms_src = source;
		} else {
			return AssetsIncludeError(
				"Unsupported file name " + entry.path().filename().string() +
				", must be vertex or fragment");
		}

		std::cout << "=> [" << style::bold << resolved_path.generic_string()
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

		AssetsBundleShaderSourceInfo source_info{0, 0, 0, 0, 0, 0};
		if (entry.second.vertex_src.has_value()) {
			source_info.vertex_pos = outfile.tellp();
			outfile << entry.second.vertex_src.value().c_str();
			source_info.vertex_size =
				static_cast<uint64_t>(outfile.tellp()) - source_info.vertex_pos;
		}
		if (entry.second.uniforms_src.has_value()) {
			source_info.uniforms_pos = outfile.tellp();
			outfile << entry.second.uniforms_src.value().c_str();
			source_info.uniforms_size = static_cast<uint64_t>(outfile.tellp()) -
										source_info.uniforms_pos;
		}
		source_info.fragment_pos = outfile.tellp();
		outfile << entry.second.fragment_src.value().c_str();
		source_info.fragment_size =
			static_cast<uint64_t>(outfile.tellp()) - source_info.fragment_pos;
		pack_info.sources[entry.first] = source_info;
	}
	info.shader_pack = pack_info;
	info.size = static_cast<uint64_t>(outfile.tellp()) - start;

	assets_map[resolved_path.generic_string()] = info;

	return AssetsIncludeErrorCode::None;
}

Error createShader(
	std::string name, std::string assets_dir, bool fragment, bool vertex,
	bool uniforms) {
	if (!(fragment || vertex)) {
		return std::make_unique<AssetsError>(
			"Either fragment or vertex must be created");
	}

	auto path = std::filesystem::path{assets_dir}.append(name);
	if (std::filesystem::exists(path)) {
		return std::make_unique<AssetsError>(
			std::string{"Path "} + path.string() + std::string{" exists."});
	}

	std::filesystem::create_directory(path);

	if (fragment) {
		std::ofstream glsl(path / "fragment.glsl", std::ios::out);
		glsl << ShaderDefaults::fragment_glsl;
		glsl.close();

		std::ofstream metal(path / "fragment.metal", std::ios::out);
		metal << ShaderDefaults::fragment_metal;
		metal.close();

		std::cout << "Created fragment shader." << std::endl;
	}

	if (vertex) {
		std::ofstream glsl(path / "vertex.glsl", std::ios::out);
		glsl << ShaderDefaults::vertex_glsl;
		glsl.close();

		std::ofstream metal(path / "vertex.metal", std::ios::out);
		metal << ShaderDefaults::vertex_metal;
		metal.close();

		std::cout << "Created vertex shader." << std::endl;
	}

	nlohmann::json j = {{".", {{"shaderPack", {{"name", name}}}}}};
	std::ofstream manifest(path / "assets.json", std::ios::out);
	manifest << j.dump(2, ' ');
	manifest.close();

	std::cout << "New shader created at " << path.string() << std::endl;

	return nullptr;
}
