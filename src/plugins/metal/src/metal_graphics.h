#pragma once

#include "metal_shader.h"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <growl/core/api/api.h>
#include <growl/core/api/system_api.h>
#include <memory>

namespace Growl {

class MetalGraphicsAPI : public GraphicsAPIInternal {
public:
	explicit MetalGraphicsAPI(SystemAPI& system);
	void init() override;
	void dispose() override;
	void begin() override;
	void end() override;
	void setWindow(WindowConfig& windowConfig) override;
	void clear(float r, float g, float b) override;
	std::unique_ptr<Texture> createTexture(Image* image) override;
	std::unique_ptr<Batch> createBatch() override;

private:
	SystemAPI& system;
	std::unique_ptr<Window> window;
	CAMetalLayer* swap_chain;
	id<CAMetalDrawable> surface;
	id<MTLDevice> device;
	id<MTLCommandQueue> command_queue;
	NSAutoreleasePool* pool;
	id<MTLCommandBuffer> command_buffer;
	std::unique_ptr<MetalShader> default_shader;
};

} // namespace Growl
