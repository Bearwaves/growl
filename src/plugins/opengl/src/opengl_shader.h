#pragma once
#include <GLES3/gl3.h>
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
	OpenGLGraphicsAPI& graphics;

	GLuint program;

	static std::string const DEFAULT_VERTEX;
	static std::string const DEFAULT_FRAGMENT;
};

} // namespace Growl
