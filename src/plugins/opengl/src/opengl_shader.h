#pragma once
#include "opengl.h"
#include <glm/gtc/type_ptr.hpp>
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

	static std::string const DEFAULT_VERTEX;
	static std::string const DEFAULT_FRAGMENT;
};

} // namespace Growl
