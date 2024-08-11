#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/system_api.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/graphics/window.h"
#include "metal_buffer.h"
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
	explicit MetalGraphicsAPI(API& api);
	Error init() override;
	void dispose() override;
	void begin() override;
	void end() override;
	Error setWindow(const Config& config) override;
	void onWindowResize(int width, int height) override;

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

	Result<std::unique_ptr<Shader>> createShader(
		const std::string& vertex_src,
		const std::string& fragment_src) override;
	Result<std::unique_ptr<Shader>>
	createShader(const std::string& fragment_src) override;
	Result<std::unique_ptr<Shader>>
	createShader(const ShaderPack& shader_pack) override;

	Window* getWindow() override {
		return window.get();
	}

	id<MTLCommandBuffer> getCommandBuffer() {
		return command_buffer;
	}

	id<CAMetalDrawable> getSurface() {
		return surface;
	}

	MetalBuffer getCurrentConstantBuffer() {
		return MetalBuffer(
			constant_buffers_ring[current_buffer], &constant_buffer_offset);
	}
	MetalBuffer getCurrentVertexBuffer() {
		return MetalBuffer(
			vertex_buffers_ring[current_buffer], &vertex_buffer_offset);
	}

private:
	API& api;
	std::unique_ptr<Window> window;
	CAMetalLayer* swap_chain;
	id<CAMetalDrawable> surface;
	id<MTLDevice> device;
	id<MTLCommandQueue> command_queue;
	NSAutoreleasePool* pool;
	id<MTLCommandBuffer> command_buffer;
#ifdef GROWL_IMGUI
	id<MTLRenderCommandEncoder> imgui_encoder;
	MTLRenderPassDescriptor* imgui_pass;
#endif
	std::unique_ptr<MetalShader> default_shader;
	std::unique_ptr<MetalShader> rect_shader;
	std::unique_ptr<MetalShader> sdf_shader;
	time_point<high_resolution_clock> last_render;
	dispatch_semaphore_t frame_boundary_semaphore;
	size_t current_buffer;
	NSArray<id<MTLBuffer>>* vertex_buffers_ring;
	size_t vertex_buffer_offset;
	NSArray<id<MTLBuffer>>* constant_buffers_ring;
	size_t constant_buffer_offset;

	std::unique_ptr<Texture>
	setupTexture(id<MTLTexture> texture, const TextureOptions options);
	const std::vector<unsigned char> convertRGBAToBGRA(const Image& rgba);
};

} // namespace Growl
