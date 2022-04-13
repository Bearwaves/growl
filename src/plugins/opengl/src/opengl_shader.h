#pragma once
#include "glm/gtc/type_ptr.hpp"
#include "opengl.h"
#include <string>

namespace Growl {

class OpenGLGraphicsAPI;

class OpenGLShader {
public:
	explicit OpenGLShader(OpenGLGraphicsAPI& graphics);
	~OpenGLShader();
	void bind(glm::mat4 mvp);

private:
	GLuint program;

	static std::string const default_vertex;
	static std::string const default_fragment;
};

} // namespace Growl
