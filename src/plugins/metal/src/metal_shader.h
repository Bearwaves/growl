#pragma once

#include <Metal/Metal.h>
#include <string>

namespace Growl {

class MetalShader {
public:
	explicit MetalShader(id<MTLDevice> device, NSString* const shader_src);
	~MetalShader();
	void bind(id<MTLTexture> dst_texture, id<MTLRenderCommandEncoder> encoder);

	static NSString* const DEFAULT_SHADER;
	static NSString* const RECT_SHADER;
	static NSString* const SDF_SHADER;

private:
	id<MTLFunction> vertex_func;
	id<MTLFunction> fragment_func;
	MTLRenderPipelineDescriptor* descriptor = nullptr;
	MTLVertexDescriptor* vertex_descriptor;

	static NSString* const GROWL_SHADER_HEADER;
};

} // namespace Growl
