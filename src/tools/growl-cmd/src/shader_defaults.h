#pragma once

#include <string>

namespace Growl {

class ShaderDefaults {
public:
	static const std::string fragment_metal;
	static const std::string fragment_glsl;
	static const std::string vertex_metal;
	static const std::string vertex_glsl;
	static const std::string uniforms_metal;
	static const std::string uniforms_glsl;
};

} // namespace Growl
