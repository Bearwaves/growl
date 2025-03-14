#include "metal_graphics.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/shader_pack.h"
#include "growl/core/error.h"
#include "growl/core/graphics/shader.h"
#include "growl/core/imgui.h"
#include "metal_error.h"
#include <Metal/Metal.h>
#ifdef GROWL_IMGUI
#include "imgui_impl_metal.h"
#endif
#include "metal_batch.h"
#include "metal_shader.h"
#include "metal_texture.h"
#include <memory>
#include <vector>

using Growl::Batch;
using Growl::Error;
using Growl::FontTextureAtlas;
using Growl::MetalError;
using Growl::MetalGraphicsAPI;
using Growl::Result;
using Growl::Shader;
using Growl::ShaderPack;
using Growl::Texture;
using Growl::TextureAtlas;
using Growl::TextureOptions;

constexpr int BUFFER_MAX_SIZE = 2 << 22; // 8MB

MetalGraphicsAPI::MetalGraphicsAPI(API& api)
	: api{api} {}

Error MetalGraphicsAPI::init() {
	last_render = high_resolution_clock::now();

	api.system().log("MetalGraphicsAPI", "Initialised Metal graphics");

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
	imGuiBegin(api);
#endif
}

void MetalGraphicsAPI::end() {
#ifdef GROWL_IMGUI
	imGuiEnd();
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

Error MetalGraphicsAPI::setWindow(const Config& config) {
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
	default_shader = std::make_unique<MetalShader>(
		device, MetalShader::default_uniforms, MetalShader::default_vertex,
		MetalShader::default_fragment);
	if (auto err = default_shader->compile()) {
		return err;
	}
	rect_shader = std::make_unique<MetalShader>(
		device, MetalShader::default_uniforms, MetalShader::default_vertex,
		MetalShader::rect_fragment);
	if (auto err = rect_shader->compile()) {
		return err;
	}
	sdf_shader = std::make_unique<MetalShader>(
		device, MetalShader::sdf_uniforms, MetalShader::default_vertex,
		MetalShader::sdf_fragment);
	if (auto err = sdf_shader->compile()) {
		return err;
	}
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

id<MTLTexture> MetalGraphicsAPI::createMetalTargetTexture(
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
	return metal_texture;
}

// Texture as render target
std::unique_ptr<Texture> MetalGraphicsAPI::createTexture(
	unsigned int width, unsigned int height, const TextureOptions options) {
	return setupTexture(
		createMetalTargetTexture(width, height, options), options);
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
	return std::make_unique<MetalBatch>(
		api, *this, default_shader.get(), rect_shader.get(), sdf_shader.get());
}

std::unique_ptr<Batch> MetalGraphicsAPI::createBatch(const Texture& texture) {
	auto& metal_texture = static_cast<const MetalTexture&>(texture);

	return std::make_unique<MetalBatch>(
		api, *this, default_shader.get(), rect_shader.get(), sdf_shader.get(),
		metal_texture.getRaw());
}

Result<std::unique_ptr<Shader>> MetalGraphicsAPI::createShader(
	const std::string& uniforms_src, const std::string& vertex_src,
	const std::string& fragment_src) {
	auto shader = std::make_unique<MetalShader>(
		device, uniforms_src, vertex_src, fragment_src);
	if (auto err = shader->compile()) {
		return Error(
			std::make_unique<MetalError>(
				"Failed to compile Metal shader: " + err->message()));
	}
	return std::unique_ptr<Shader>(std::move(shader));
}

Result<std::unique_ptr<Shader>> MetalGraphicsAPI::createShader(
	const std::string& uniforms_src, const std::string& fragment_src) {
	return createShader(
		uniforms_src, MetalShader::default_vertex, fragment_src);
}

Result<std::unique_ptr<Shader>>
MetalGraphicsAPI::createShader(const std::string& fragment_src) {
	return createShader(
		MetalShader::default_uniforms, MetalShader::default_vertex,
		fragment_src);
}

Result<std::unique_ptr<Shader>>
MetalGraphicsAPI::createShader(const ShaderPack& shader_pack) {
	auto source = shader_pack.getSources().find(ShaderType::Metal);
	if (source == shader_pack.getSources().end()) {
		return Error(
			std::make_unique<MetalError>(
				"No Metal sources found in shader pack"));
	}
	if (!source->second.fragment_src.has_value()) {
		return Error(
			std::make_unique<MetalError>(
				"No fragment source found in shader pack"));
	}
	return createShader(
		source->second.uniforms_src.has_value()
			? source->second.uniforms_src.value()
			: MetalShader::default_uniforms,
		source->second.vertex_src.has_value()
			? source->second.vertex_src.value()
			: MetalShader::default_vertex,
		source->second.fragment_src.value());
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
