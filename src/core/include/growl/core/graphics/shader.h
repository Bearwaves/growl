#pragma once

#include "growl/core/error.h"
namespace Growl {

class Shader {
public:
	Shader(const std::string& vertex_src, const std::string& fragment_src)
		: vertex_src{vertex_src}
		, fragment_src{fragment_src} {};
	virtual ~Shader() = default;

	// Shader is move-only
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;

	virtual Error compile() = 0;

	virtual const std::string& getVertexSource() {
		return vertex_src;
	}

	virtual const std::string& getFragmentSource() {
		return fragment_src;
	}

	virtual void setVertexSource(const std::string& src) {
		vertex_src = src;
	}

	virtual void setFragmentSource(const std::string& src) {
		fragment_src = src;
	}

protected:
	std::string vertex_src;
	std::string fragment_src;
};

} // namespace Growl
