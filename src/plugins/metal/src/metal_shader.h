#pragma once

#include <Metal/Metal.h>
#include <string>

namespace Growl {

class MetalShader {
public:
	explicit MetalShader(id<MTLDevice> device);
	~MetalShader();
	void bind(id<MTLTexture> dstTexture, id<MTLRenderCommandEncoder> encoder);

private:
	id<MTLFunction> vertex_func;
	id<MTLFunction> fragment_func;
	MTLRenderPipelineDescriptor* descriptor = nullptr;
	MTLVertexDescriptor* vertex_descriptor;

	static NSString* const DEFAULT_SHADER;
};

}
