#include "metal_shader.h"
#include <cassert>

using Growl::MetalShader;

MetalShader::MetalShader(id<MTLDevice> device, NSString* const shader_src) {
	auto compile_options = [MTLCompileOptions new];
	NSError* compile_error;
	id<MTLLibrary> lib =
		[device newLibraryWithSource:[GROWL_SHADER_HEADER
										 stringByAppendingString:shader_src]
							 options:compile_options
							   error:&compile_error];
	assert(!compile_error);
	fragment_func = [lib newFunctionWithName:@"pixel_func"];
	vertex_func = [lib newFunctionWithName:@"vertex_func"];
	[lib release];
	[compile_error release];
	[compile_options release];
}

MetalShader::~MetalShader() {
	[vertex_func release];
	[fragment_func release];
	if (descriptor) {
		[descriptor release];
	}
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

NSString* const MetalShader::GROWL_SHADER_HEADER = @R"(
#include <metal_stdlib>
using namespace metal;

struct ConstantBlock {
	float4x4 mvp;
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

NSString* const MetalShader::DEFAULT_SHADER = @R"(
vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.position = constant_block.mvp * float4(v.position, 0, 1);
	outVertex.color = v.color;

	return outVertex;
}

fragment float4 pixel_func (
	VertexOut v [[ stage_in ]],
	texture2d<float> tex0 [[ texture(0) ]],
	sampler sampler0 [[ sampler(0) ]]
) {
	return tex0.sample(sampler0, v.texCoord0) * v.color;
}
)";

NSString* const MetalShader::RECT_SHADER = @R"(
vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.position = constant_block.mvp * float4(v.position, 0, 1);
	outVertex.color = v.color;

	return outVertex;
}

fragment float4 pixel_func (
	VertexOut v [[ stage_in ]]
) {
	return v.color;
}
)";

NSString* const MetalShader::SDF_SHADER = @R"(
vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.position = constant_block.mvp * float4(v.position, 0, 1);
	outVertex.color = v.color;

	return outVertex;
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

fragment float4 pixel_func (
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
