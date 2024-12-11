#pragma once

#include "growl/core/graphics/shader.h"
#include <Metal/Metal.h>
#include <string>

namespace Growl {

class MetalShader : public Shader {
public:
	explicit MetalShader(
		id<MTLDevice> device, const std::string& uniforms_src,
		const std::string& vertex_src, const std::string& fragment_src)
		: Shader(uniforms_src, vertex_src, fragment_src)
		, device{device} {}
	~MetalShader();
	void bind(id<MTLTexture> dst_texture, id<MTLRenderCommandEncoder> encoder);

	Error compile() override;

	static const std::string default_uniforms;
	static const std::string default_vertex;
	static const std::string default_fragment;
	static const std::string sdf_uniforms;
	static const std::string sdf_fragment;
	static const std::string rect_fragment;

private:
	id<MTLDevice> device;
	id<MTLFunction> vertex_func;
	id<MTLFunction> fragment_func;
	MTLRenderPipelineDescriptor* descriptor = nullptr;
	MTLVertexDescriptor* vertex_descriptor;

	static const std::string growl_shader_header;
};

} // namespace Growl
