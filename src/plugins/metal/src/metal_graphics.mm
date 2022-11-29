#include "metal_graphics.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/mat4x4.hpp"
#include "growl/core/assets/font_face.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_sdl.h"
#endif
#include "metal_batch.h"
#include "metal_shader.h"
#include "metal_texture.h"
#include <memory>
#include <vector>

using Growl::Error;
using Growl::MetalGraphicsAPI;
using Growl::Texture;
using Growl::TextureAtlas;
using Growl::FontTextureAtlas;
using Growl::TextureOptions;
using Growl::Batch;
using std::chrono::duration;
using std::chrono::seconds;

constexpr int BUFFER_MAX_SIZE = 2 << 22; // 8MB

MetalGraphicsAPI::MetalGraphicsAPI(API& api)
	: api{api} {}

Error MetalGraphicsAPI::init() {
	last_render = high_resolution_clock::now();
	return nullptr;
}

void MetalGraphicsAPI::dispose() {
#ifdef GROWL_IMGUI
	ImGui_ImplMetal_Shutdown();
#endif
}

void MetalGraphicsAPI::begin() {
	pool = [[NSAutoreleasePool alloc] init];
	dispatch_semaphore_wait(frame_boundary_semaphore, DISPATCH_TIME_FOREVER);
	surface = [swap_chain nextDrawable];
	command_buffer = [command_queue commandBuffer];
	auto tp = high_resolution_clock::now();
	deltaTime = duration<double, seconds::period>(tp - last_render).count();
	last_render = tp;
	current_buffer = (current_buffer + 1) % swap_chain.maximumDrawableCount;
	constant_buffer_offset = 0;
	vertex_buffer_offset = 0;

#ifdef GROWL_IMGUI
	// ImGui
	imgui_pass = [MTLRenderPassDescriptor renderPassDescriptor];
	imgui_pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
	imgui_pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	imgui_pass.colorAttachments[0].texture = surface.texture;

	// Start the Dear ImGui frame
	ImGui_ImplMetal_NewFrame(imgui_pass);
	window->newImguiFrame();
	ImGui::NewFrame();
#endif
}

void MetalGraphicsAPI::end() {
#ifdef GROWL_IMGUI
	ImGui::Render();
	if (api.imguiVisible()) {
		imgui_encoder =
			[command_buffer renderCommandEncoderWithDescriptor:imgui_pass];
		[imgui_encoder pushDebugGroup:@"ImGui"];
		ImGui_ImplMetal_RenderDrawData(
			ImGui::GetDrawData(), command_buffer, imgui_encoder);
		[imgui_encoder popDebugGroup];
		[imgui_encoder endEncoding];
	}
#endif

	[command_buffer presentDrawable:surface];
	[command_buffer addCompletedHandler:^(id<MTLCommandBuffer> command_buffer) {
	  dispatch_semaphore_signal(frame_boundary_semaphore);
	}];
	[command_buffer commit];
	[pool release];
}

Error MetalGraphicsAPI::setWindow(const WindowConfig& config) {
	auto window_result = api.system().createWindow(config);
	if (window_result.hasError()) {
		return std::move(window_result.error());
	}
	window = std::move(window_result.get());
	swap_chain = static_cast<CAMetalLayer*>(window->getMetalLayer());
	swap_chain.pixelFormat = MTLPixelFormatBGRA8Unorm;
	device = swap_chain.device;
#ifdef GROWL_IMGUI
	ImGui_ImplMetal_Init(device);
	window->initImgui();
#endif
	frame_boundary_semaphore =
		dispatch_semaphore_create(swap_chain.maximumDrawableCount);
	current_buffer = 0;
	NSMutableArray* constant_buffers = [[NSMutableArray
		arrayWithCapacity:swap_chain.maximumDrawableCount] retain];
	for (int i = 0; i < swap_chain.maximumDrawableCount; i++) {
		id<MTLBuffer> buffer =
			[device newBufferWithLength:BUFFER_MAX_SIZE
								options:MTLResourceStorageModeShared];
		[constant_buffers addObject:buffer];
	}
	constant_buffers_ring = constant_buffers;
	NSMutableArray* vertex_buffers = [[NSMutableArray
		arrayWithCapacity:swap_chain.maximumDrawableCount] retain];
	for (int i = 0; i < swap_chain.maximumDrawableCount; i++) {
		id<MTLBuffer> buffer =
			[device newBufferWithLength:BUFFER_MAX_SIZE
								options:MTLResourceStorageModeShared];
		[vertex_buffers addObject:buffer];
	}
	vertex_buffers_ring = vertex_buffers;
	command_queue = [device newCommandQueue];
	default_shader =
		std::make_unique<MetalShader>(device, MetalShader::DEFAULT_SHADER);
	rect_shader =
		std::make_unique<MetalShader>(device, MetalShader::RECT_SHADER);
	sdf_shader = std::make_unique<MetalShader>(device, MetalShader::SDF_SHADER);
	return nullptr;
}

void MetalGraphicsAPI::onWindowResize(int width, int height) {
	[swap_chain setDrawableSize:CGSize{
									.width = static_cast<CGFloat>(width),
									.height = static_cast<CGFloat>(height)}];
}

std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(
	const Image& image, const TextureOptions options) {
	auto texture_descriptor = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
									 width:image.getWidth()
									height:image.getHeight()
								 mipmapped:options.mipmapped];
	auto metal_texture = [device newTextureWithDescriptor:texture_descriptor];
	NSUInteger bytes_per_row = 4 * image.getWidth();
	MTLRegion region = {
		{0, 0, 0},
		{static_cast<NSUInteger>(image.getWidth()),
		 static_cast<NSUInteger>(image.getHeight()), 1}};
	std::vector<unsigned char> bgra = convertRGBAToBGRA(image);
	[metal_texture replaceRegion:region
					 mipmapLevel:0
					   withBytes:bgra.data()
					 bytesPerRow:bytes_per_row];

	return setupTexture(metal_texture, options);
}

// Texture as render target
std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(
	unsigned int width, unsigned int height, const TextureOptions options) {
	auto texture_descriptor = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
									 width:width
									height:height
								 mipmapped:options.mipmapped];
	texture_descriptor.usage =
		MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
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
	return std::make_unique<TextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<FontTextureAtlas>
MetalGraphicsAPI::createFontTextureAtlas(const FontFace& face) {
	bool is_msdf = face.getType() != FontFaceType::Bitmap;
	return std::make_unique<FontTextureAtlas>(
		face, createTexture(face.getImage(), {is_msdf, false}));
}

std::unique_ptr<Batch> MetalGraphicsAPI::createBatch() {
	auto projection = glm::ortho<float>(
		0, surface.texture.width, surface.texture.height, 0, 1, -1);
	auto buffer = constant_buffers_ring[current_buffer];

	return std::make_unique<MetalBatch>(
		command_buffer, surface.texture, default_shader.get(),
		rect_shader.get(), sdf_shader.get(), projection, buffer,
		&constant_buffer_offset, vertex_buffers_ring[current_buffer],
		&vertex_buffer_offset);
}

std::unique_ptr<Batch> MetalGraphicsAPI::createBatch(const Texture& texture) {
	auto& metal_texture = static_cast<const MetalTexture&>(texture);

	auto projection = glm::ortho<float>(
		0, metal_texture.getWidth(), metal_texture.getHeight(), 0, 1, -1);
	auto buffer = constant_buffers_ring[current_buffer];

	return std::make_unique<MetalBatch>(
		command_buffer, metal_texture.getRaw(), default_shader.get(),
		rect_shader.get(), sdf_shader.get(), projection, buffer,
		&constant_buffer_offset, vertex_buffers_ring[current_buffer],
		&vertex_buffer_offset);
}

const std::vector<unsigned char>
MetalGraphicsAPI::convertRGBAToBGRA(const Image& rgba) {
	const unsigned char* src = rgba.getRaw();
	std::vector<unsigned char> output(
		4 * rgba.getWidth() * rgba.getHeight(), 0);
	for (int i = 0; i < rgba.getWidth() * rgba.getHeight(); i++) {
		unsigned char* dst = output.data() + i * 4;
		unsigned char r = *src++, g = *src++, b = *src++, a = *src++;
		*dst++ = b;
		*dst++ = g;
		*dst++ = r;
		*dst++ = a;
	}
	return output;
}
