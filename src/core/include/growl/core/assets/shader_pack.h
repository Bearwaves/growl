#pragma once

#include <optional>
#include <string>
#include <unordered_map>
namespace Growl {

enum class ShaderType { GLSL, Metal };

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
	virtual ~ShaderPack() = default;

	const std::unordered_map<ShaderType, ShaderSource>& getSources() const {
		return sources;
	}

private:
	std::string name;
	std::unordered_map<ShaderType, ShaderSource> sources;
};

} // namespace Growl
