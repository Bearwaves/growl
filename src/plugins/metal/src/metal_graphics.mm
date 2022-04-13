#include "metal_graphics.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/mat4x4.hpp"
#include "metal_batch.h"
#include "metal_texture.h"
#include "metal_texture_atlas.h"
#include <SDL.h>
#include <vector>

using Growl::Error;
using Growl::MetalGraphicsAPI;
using Growl::Texture;
using Growl::TextureAtlas;
using Growl::FontTextureAtlas;
using Growl::TextureOptions;
using Growl::MetalTextureAtlas;
using Growl::Batch;
using std::chrono::duration;
using std::chrono::seconds;

MetalGraphicsAPI::MetalGraphicsAPI(SystemAPI* system)
	: system{system} {}

Error MetalGraphicsAPI::init() {
	last_render = high_resolution_clock::now();
	return nullptr;
}

void MetalGraphicsAPI::dispose() {}

void MetalGraphicsAPI::begin() {
	pool = [[NSAutoreleasePool alloc] init];
	surface = [swap_chain nextDrawable];
	command_buffer = [command_queue commandBuffer];
	auto tp = high_resolution_clock::now();
	deltaTime = duration<double, seconds::period>(tp - last_render).count();
	last_render = tp;
}

void MetalGraphicsAPI::end() {
	[command_buffer presentDrawable:surface];
	[command_buffer commit];
	[pool release];
}

Error MetalGraphicsAPI::setWindow(const WindowConfig& config) {
	auto window_result = system->createWindow(config);
	if (window_result.hasError()) {
		return std::move(window_result.error());
	}
	window = std::move(window_result.get());
	SDL_Window* native_window = static_cast<SDL_Window*>(window->getNative());
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
	SDL_Renderer* renderer =
		SDL_CreateRenderer(native_window, -1, SDL_RENDERER_PRESENTVSYNC);
	swap_chain = static_cast<CAMetalLayer*>(SDL_RenderGetMetalLayer(renderer));
	SDL_DestroyRenderer(renderer);
	swap_chain.pixelFormat = MTLPixelFormatBGRA8Unorm;
	device = swap_chain.device;
	command_queue = [device newCommandQueue];
	default_shader = std::make_unique<MetalShader>(device);
	return nullptr;
}

void MetalGraphicsAPI::clear(float r, float g, float b) {
	MTLRenderPassDescriptor* pass =
		[MTLRenderPassDescriptor renderPassDescriptor];
	pass.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, 1);
	pass.colorAttachments[0].loadAction = MTLLoadActionClear;
	pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	pass.colorAttachments[0].texture = surface.texture;

	id<MTLRenderCommandEncoder> encoder =
		[command_buffer renderCommandEncoderWithDescriptor:pass];
	[encoder endEncoding];
}

std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(
	const Image& image, const TextureOptions options) {
	auto texture_descriptor = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
									 width:image.getWidth()
									height:image.getHeight()
								 mipmapped:options.mipmapped];
	auto metal_texture = [device newTextureWithDescriptor:texture_descriptor];
	NSUInteger bytes_per_row = 4 * image.getWidth();
	MTLRegion region = {
		{0, 0, 0},
		{static_cast<NSUInteger>(image.getWidth()),
		 static_cast<NSUInteger>(image.getHeight()), 1}};
	[metal_texture replaceRegion:region
					 mipmapLevel:0
					   withBytes:image.getRaw()
					 bytesPerRow:bytes_per_row];

	return setupTexture(metal_texture, options);
}

std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(
	unsigned int width, unsigned int height, const TextureOptions options) {
	auto texture_descriptor = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
									 width:width
									height:height
								 mipmapped:options.mipmapped];
	auto metal_texture = [device newTextureWithDescriptor:texture_descriptor];
	std::vector<unsigned char> bytes(4 * width * height, 0);
	MTLRegion region = {{0, 0, 0}, {width, height, 1}};
	[metal_texture replaceRegion:region
					 mipmapLevel:0
					   withBytes:bytes.data()
					 bytesPerRow:4 * width];
	return setupTexture(metal_texture, options);
}

std::unique_ptr<Texture> MetalGraphicsAPI::setupTexture(
	id<MTLTexture> texture, const TextureOptions options) {
	if (options.mipmapped) {
		auto buf = [command_queue commandBuffer];
		auto blit_encoder = [buf blitCommandEncoder];
		[blit_encoder generateMipmapsForTexture:texture];
		[blit_encoder endEncoding];
		[buf commit];
	}

	MTLSamplerDescriptor* sampler_descriptor =
		[[MTLSamplerDescriptor alloc] init];
	sampler_descriptor.maxAnisotropy = 1;
	auto filter = options.filtering ? MTLSamplerMinMagFilterLinear
									: MTLSamplerMinMagFilterNearest;
	sampler_descriptor.minFilter = filter;
	sampler_descriptor.magFilter = filter;
	if (options.mipmapped) {
		sampler_descriptor.mipFilter = options.filtering
										   ? MTLSamplerMipFilterLinear
										   : MTLSamplerMipFilterNearest;
	}
	auto address_mode = MTLSamplerAddressModeClampToEdge;
	sampler_descriptor.sAddressMode = address_mode;
	sampler_descriptor.rAddressMode = address_mode;
	sampler_descriptor.tAddressMode = address_mode;
	sampler_descriptor.lodMinClamp = 0;
	sampler_descriptor.lodMaxClamp = FLT_MAX;
	auto sampler = [device newSamplerStateWithDescriptor:sampler_descriptor];
	[sampler_descriptor release];

	return std::make_unique<MetalTexture>(
		texture, sampler, texture.width, texture.height);
}

std::unique_ptr<TextureAtlas> MetalGraphicsAPI::createTextureAtlas(
	const Atlas& atlas, const TextureOptions options) {
	return std::make_unique<MetalTextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<FontTextureAtlas> MetalGraphicsAPI::createFontTextureAtlas(
	const FontAtlas& atlas, const TextureOptions options) {
	return std::make_unique<FontTextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<Batch> MetalGraphicsAPI::createBatch() {
	auto projection = glm::ortho<float>(
		0, surface.texture.width, surface.texture.height, 0, 1, -1);
	auto buffer = [device newBufferWithBytes:&projection
									  length:sizeof(projection)
									 options:MTLResourceStorageModeShared];
	return std::make_unique<MetalBatch>(
		command_buffer, surface.texture, default_shader.get(), buffer);
}

std::unique_ptr<Batch> MetalGraphicsAPI::createBatch(const Texture& texture) {
	auto& metal_texture = static_cast<const MetalTexture&>(texture);

	auto projection = glm::ortho<float>(
		0, metal_texture.getWidth(), metal_texture.getHeight(), 0, 1, -1);
	auto buffer = [device newBufferWithBytes:&projection
									  length:sizeof(projection)
									 options:MTLResourceStorageModeShared];
	return std::make_unique<MetalBatch>(
		command_buffer, metal_texture.getRaw(), default_shader.get(), buffer);
}
