#pragma once
#include "growl/core/graphics/shader.h"
#include "opengl.h"
#include <string>

namespace Growl {

struct Color;
class OpenGLGraphicsAPI;

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertex_src, const std::string& fragment_src)
		: Shader(vertex_src, fragment_src) {}
	~OpenGLShader();

	Error compile() override;

	void bind(Color color);

	static const std::string default_vertex;
	static const std::string default_fragment;
	static const std::string sdf_fragment;
	static const std::string rect_fragment;

private:
	GLuint program;
	static std::string const header;

	Error checkShaderCompileError(unsigned int shader);
};

} // namespace Growl
