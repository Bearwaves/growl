#pragma once

#include "growl/core/graphics/shader_type.h"
#include <optional>
#include <string>
#include <unordered_map>

namespace Growl {

struct ShaderSource {
	std::optional<std::string> uniforms_src;
	std::optional<std::string> vertex_src;
	std::optional<std::string> fragment_src;
};

// A ShaderPack contains shader sources for various different backends.
class ShaderPack {
public:
	ShaderPack(
		std::string name, std::unordered_map<ShaderType, ShaderSource> sources)
		: name{name}
		, sources{std::move(sources)} {}
	ShaderPack(
		std::string name, std::string developer_relative_path,
		std::unordered_map<ShaderType, ShaderSource> sources)
		: name{name}
		, developer_relative_path{developer_relative_path}
		, sources{std::move(sources)} {}
	virtual ~ShaderPack() = default;

	const std::string& getName() const {
		return name;
	}

	const std::string& getDeveloperRelativePath() const {
		return developer_relative_path;
	}

	const std::unordered_map<ShaderType, ShaderSource>& getSources() const {
		return sources;
	}

private:
	std::string name;
	std::string developer_relative_path;
	std::unordered_map<ShaderType, ShaderSource> sources;
};

} // namespace Growl
