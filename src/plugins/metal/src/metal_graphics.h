#pragma once

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <growl/core/api/api.h>
#include <growl/core/api/system_api.h>

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

private:
	SystemAPI& system;
	std::shared_ptr<Window> window;
	CAMetalLayer* swap_chain;
	id<CAMetalDrawable> surface;
	id<MTLDevice> device;
	id<MTLCommandQueue> command_queue;
	NSAutoreleasePool* pool;
	id<MTLCommandBuffer> command_buffer;
};

} // namespace Growl
