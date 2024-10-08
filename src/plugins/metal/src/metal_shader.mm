#include "metal_shader.h"
#include "growl/core/error.h"
#include "metal_error.h"
#include <cassert>

using Growl::Error;
using Growl::MetalError;
using Growl::MetalShader;

MetalShader::~MetalShader() {
	[vertex_func release];
	[fragment_func release];
	if (descriptor) {
		[descriptor release];
	}
}

Error MetalShader::compile() {
	@autoreleasepool {
		auto compile_options = [[MTLCompileOptions new] autorelease];
		NSError* compile_error = nil;
		[compile_error autorelease];
		std::string src =
			growl_shader_header + uniforms_src + vertex_src + fragment_src;
		id<MTLLibrary> lib = [[device
			newLibraryWithSource:
				[NSString stringWithCString:src.c_str()
								   encoding:[NSString defaultCStringEncoding]]
						 options:compile_options
						   error:&compile_error] autorelease];
		if (compile_error) {
			return std::make_unique<MetalError>(compile_error);
		}
		[vertex_func release];
		[fragment_func release];
		fragment_func = [lib newFunctionWithName:@"fragment_func"];
		vertex_func = [lib newFunctionWithName:@"vertex_func"];
		if (descriptor) {
			[descriptor release];
			descriptor = nil;
		}
	}
	return nullptr;
}

void MetalShader::bind(
	id<MTLTexture> dst_texture, id<MTLRenderCommandEncoder> encoder) {
	if (!descriptor) {
		descriptor = [[MTLRenderPipelineDescriptor alloc] init];
		descriptor.vertexFunction = vertex_func;
		descriptor.fragmentFunction = fragment_func;
		descriptor.label = @"Growl shader";
		descriptor.colorAttachments[0].pixelFormat = dst_texture.pixelFormat;
		descriptor.colorAttachments[0].blendingEnabled = true;
		descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
		descriptor.colorAttachments[0].alphaBlendOperation =
			MTLBlendOperationAdd;
		descriptor.colorAttachments[0].sourceRGBBlendFactor =
			MTLBlendFactorSourceAlpha;
		descriptor.colorAttachments[0].sourceAlphaBlendFactor =
			MTLBlendFactorSourceAlpha;
		descriptor.colorAttachments[0].destinationRGBBlendFactor =
			MTLBlendFactorOneMinusSourceAlpha;
		descriptor.colorAttachments[0].destinationAlphaBlendFactor =
			MTLBlendFactorOneMinusSourceAlpha;
		descriptor.vertexBuffers[0].mutability = MTLMutabilityImmutable;
		descriptor.vertexBuffers[1].mutability = MTLMutabilityImmutable;

		vertex_descriptor = [[MTLVertexDescriptor alloc] init];
		descriptor.vertexDescriptor = vertex_descriptor;
	}

	NSError* error = NULL;
	id<MTLRenderPipelineState> pipeline_state = [[encoder.device
		newRenderPipelineStateWithDescriptor:descriptor
									   error:&error] autorelease];
	assert(!error);
	[error release];
	[encoder setRenderPipelineState:pipeline_state];
}

const std::string MetalShader::growl_shader_header = R"(
#include <metal_stdlib>
using namespace metal;

struct ConstantBlock {
	float4x4 projection;
	float2 resolution;
	float time;
	float deltaTime;
};

struct VertexIn {
	float2 position;
	float2 vertPos;
	float4 color;
};

struct VertexOut {
	float2 texCoord0;
	float4 position [[ position ]];
	float4 color;
};
)";

const std::string MetalShader::default_uniforms = R"(
struct Uniforms {};
)";

const std::string MetalShader::default_vertex = R"(
vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	constant float4x4& transform [[ buffer(2) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.position = constant_block.projection * transform * float4(v.position, 0, 1);
	outVertex.color = v.color;

	return outVertex;
}
)";

const std::string MetalShader::default_fragment = R"(
fragment float4 fragment_func (
	VertexOut v [[ stage_in ]],
	texture2d<float> tex0 [[ texture(0) ]],
	sampler sampler0 [[ sampler(0) ]]
) {
	return tex0.sample(sampler0, v.texCoord0) * v.color;
}
)";

const std::string MetalShader::rect_uniforms = R"(
struct Uniforms {
	float2 rect_size;
	float border_size;
};
)";

const std::string MetalShader::rect_fragment = R"(
fragment float4 fragment_func (
	VertexOut v [[ stage_in ]],
	constant Uniforms& uniforms [[ buffer(2) ]]
) {
	float2 border_uv = uniforms.border_size / uniforms.rect_size;
	float2 dist = min(v.texCoord0, 1.0 - v.texCoord0);
	float inside = step(border_uv.x, dist.x) * step(border_uv.y, dist.y);
	float4 fill_color = mix(v.color, float4(0.0), step(1.0, uniforms.border_size));
	return mix(v.color, fill_color, inside);
}
)";

const std::string MetalShader::sdf_fragment = R"(
float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

fragment float4 fragment_func (
	VertexOut v [[ stage_in ]],
	texture2d<float> tex0 [[ texture(0) ]],
	sampler sampler0 [[ sampler(0) ]]
) {
	float4 msd = tex0.sample(sampler0, v.texCoord0);
	float sd = median(msd.r, msd.g, msd.b) - 0.5;
	float d = fwidth(sd);
	float opacity = smoothstep(-d, d, sd);
	return float4(v.color.rgb, v.color.a * opacity);
}
)";
