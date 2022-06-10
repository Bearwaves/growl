#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/system_api.h"
#include "growl/util/assets/font_face.h"
#include "metal_shader.h"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <chrono>
#include <memory>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;

namespace Growl {

class MetalGraphicsAPI : public GraphicsAPIInternal {
public:
	explicit MetalGraphicsAPI(SystemAPI* system);
	Error init() override;
	void dispose() override;
	void begin() override;
	void end() override;
	Error setWindow(const WindowConfig& window_config) override;
	void clear(float r, float g, float b) override;

	std::unique_ptr<Texture>
	createTexture(const Image& image, const TextureOptions options) override;
	std::unique_ptr<Texture> createTexture(
		unsigned int width, unsigned int height,
		const TextureOptions options) override;

	std::unique_ptr<TextureAtlas> createTextureAtlas(
		const Atlas& atlas, const TextureOptions options) override;

	std::unique_ptr<FontTextureAtlas>
	createFontTextureAtlas(const FontFace& face) override;

	std::unique_ptr<Batch> createBatch() override;
	std::unique_ptr<Batch> createBatch(const Texture& texture) override;

private:
	SystemAPI* system;
	std::unique_ptr<Window> window;
	CAMetalLayer* swap_chain;
	id<CAMetalDrawable> surface;
	id<MTLDevice> device;
	id<MTLCommandQueue> command_queue;
	NSAutoreleasePool* pool;
	id<MTLCommandBuffer> command_buffer;
	std::unique_ptr<MetalShader> default_shader;
	std::unique_ptr<MetalShader> rect_shader;
	std::unique_ptr<MetalShader> sdf_shader;
	time_point<high_resolution_clock> last_render;
	dispatch_semaphore_t frame_boundary_semaphore;
	uint32_t current_buffer;
	NSArray<id<MTLBuffer>>* vertex_buffers_ring;
	uint32_t vertex_buffer_offset;
	NSArray<id<MTLBuffer>>* constant_buffers_ring;
	uint32_t constant_buffer_offset;

	std::unique_ptr<Texture>
	setupTexture(id<MTLTexture> texture, const TextureOptions options);
	const std::vector<unsigned char> convertRGBAToBGRA(const Image& rgba);
};

} // namespace Growl
