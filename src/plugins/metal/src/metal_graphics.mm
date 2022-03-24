#include "metal_graphics.h"
#include "metal_batch.h"
#include "metal_texture.h"
#include "metal_texture_atlas.h"
#include <SDL.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>

using Growl::Error;
using Growl::MetalGraphicsAPI;
using Growl::Texture;
using Growl::TextureAtlas;
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
	auto windowResult = system->createWindow(config);
	if (windowResult.hasError()) {
		return std::move(windowResult.error());
	}
	window = std::move(windowResult.get());
	SDL_Window* nativeWindow = static_cast<SDL_Window*>(window->getNative());
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
	SDL_Renderer* renderer =
		SDL_CreateRenderer(nativeWindow, -1, SDL_RENDERER_PRESENTVSYNC);
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

std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(const Image& image) {
	auto textureDescriptor = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
									 width:image.getWidth()
									height:image.getHeight()
								 mipmapped:true];
	auto metalTexture = [device newTextureWithDescriptor:textureDescriptor];
	NSUInteger bytesPerRow = 4 * image.getWidth();
	MTLRegion region = {
		{0, 0, 0},
		{static_cast<NSUInteger>(image.getWidth()),
		 static_cast<NSUInteger>(image.getHeight()), 1}};
	const Byte* imageBytes = image.getRaw();
	[metalTexture replaceRegion:region
					mipmapLevel:0
					  withBytes:imageBytes
					bytesPerRow:bytesPerRow];

	auto buf = [command_queue commandBuffer];
	auto blitEncoder = [buf blitCommandEncoder];
	[blitEncoder generateMipmapsForTexture:metalTexture];
	[blitEncoder endEncoding];
	[buf commit];

	MTLSamplerDescriptor* samplerDescriptor =
		[[MTLSamplerDescriptor alloc] init];
	samplerDescriptor.maxAnisotropy = 1;
	auto filter = image.useFiltering() ? MTLSamplerMinMagFilterLinear
									   : MTLSamplerMinMagFilterNearest;
	samplerDescriptor.minFilter = filter;
	samplerDescriptor.magFilter = filter;
	samplerDescriptor.mipFilter = image.useFiltering()
									  ? MTLSamplerMipFilterLinear
									  : MTLSamplerMipFilterNearest;
	auto addressMode = MTLSamplerAddressModeClampToEdge;
	samplerDescriptor.sAddressMode = addressMode;
	samplerDescriptor.rAddressMode = addressMode;
	samplerDescriptor.tAddressMode = addressMode;
	samplerDescriptor.lodMinClamp = 0;
	samplerDescriptor.lodMaxClamp = FLT_MAX;
	auto sampler = [device newSamplerStateWithDescriptor:samplerDescriptor];
	[samplerDescriptor release];

	return std::make_unique<MetalTexture>(
		metalTexture, sampler, image.getWidth(), image.getHeight());
}

std::unique_ptr<TextureAtlas>
MetalGraphicsAPI::createTextureAtlas(const Atlas& atlas) {
	return std::make_unique<MetalTextureAtlas>(
		atlas, createTexture(atlas.getImage()));
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
