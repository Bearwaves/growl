#pragma once
#include "glm/gtc/type_ptr.hpp"
#include "growl/core/graphics/color.h"
#include "opengl.h"
#include <string>

namespace Growl {

class OpenGLGraphicsAPI;

class OpenGLShader {
public:
	explicit OpenGLShader(
		OpenGLGraphicsAPI& graphics, std::string vertex_src = default_vertex,
		std::string fragment_src = default_fragment);
	~OpenGLShader();
	void bind(glm::mat4 mvp, Color color);

	static std::string const default_vertex;
	static std::string const default_fragment;
	static std::string const sdf_fragment;
	static std::string const rect_fragment;

private:
	GLuint program;
	static std::string const header;
};

} // namespace Growl
