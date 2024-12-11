#pragma once
#include "growl/core/graphics/shader.h"
#include "opengl.h"
#include <string>

namespace Growl {

struct Color;
class OpenGLGraphicsAPI;

class OpenGLShader : public Shader {
public:
	OpenGLShader(
		const std::string& uniforms_src, const std::string& vertex_src,
		const std::string& fragment_src)
		: Shader(uniforms_src, vertex_src, fragment_src) {}
	~OpenGLShader();

	Error compile() override;

	void bind();

	static const std::string default_uniforms;
	static const std::string default_vertex;
	static const std::string default_fragment;
	static const std::string sdf_uniforms;
	static const std::string sdf_fragment;
	static const std::string rect_fragment;

private:
	GLuint program;
	static const std::string header;
	static const std::string vertex_block;
	static const std::string fragment_block;

	Error checkShaderCompileError(unsigned int shader);
};

} // namespace Growl
