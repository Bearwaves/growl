#pragma once

#include <Metal/Metal.h>
#include <growl/core/graphics/batch.h>

#include "metal_shader.h"

namespace Growl {

class MetalBatch : public Batch {
public:
	MetalBatch(
		id<MTLCommandBuffer> command_buffer, id<MTLTexture> surface,
		MetalShader* shader, id<MTLBuffer> constant_buffer)
		: command_buffer{command_buffer}
		, surface{surface}
		, shader{shader}
		, constant_buffer{constant_buffer} {}
	void begin() override;
	void end() override;
	void draw(
		Texture* texture, float x, float y, float width, float height) override;

private:
	id<MTLCommandBuffer> command_buffer;
	id<MTLTexture> surface;
	id<MTLRenderCommandEncoder> encoder;
	MetalShader* shader;
	MTLRenderPassDescriptor* renderPassDescriptor();
	id<MTLBuffer> constant_buffer;
};

} // namespace Growl
