#pragma once

#include <Metal/Metal.h>
#include <growl/core/graphics/batch.h>

#include "metal_shader.h"

namespace Growl {

class MetalBatch : public Batch {
public:
	MetalBatch(
		id<MTLCommandBuffer> command_buffer, id<MTLTexture> surface,
		MetalShader* shader)
		: command_buffer{command_buffer}
		, surface{surface}
		, shader{shader} {}
	void begin() override;
	void end() override;
	void draw(Texture* texture, int x, int y, int width, int height) override;

private:
	id<MTLCommandBuffer> command_buffer;
	id<MTLTexture> surface;
	id<MTLRenderCommandEncoder> encoder;
	MetalShader* shader;
	MTLRenderPassDescriptor* renderPassDescriptor();
};

} // namespace Growl
