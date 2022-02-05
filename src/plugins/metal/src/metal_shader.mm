#include "metal_shader.h"
#include <cassert>

using Growl::MetalShader;

MetalShader::MetalShader(id<MTLDevice> device) {
	auto compileOptions = [MTLCompileOptions new];
	NSError* compileError;
	id<MTLLibrary> lib = [device newLibraryWithSource:DEFAULT_SHADER
											  options:compileOptions
												error:&compileError];
	assert(!compileError);
	fragment_func = [lib newFunctionWithName:@"pixel_func"];
	vertex_func = [lib newFunctionWithName:@"vertex_func"];
	[lib release];
	[compileError release];
	[compileOptions release];
}

MetalShader::~MetalShader() {
	[vertex_func release];
	[fragment_func release];
	if (descriptor) {
		[descriptor release];
	}
}

void MetalShader::bind(
	id<MTLTexture> dstTexture, id<MTLRenderCommandEncoder> encoder) {
	if (!descriptor) {
		descriptor = [[MTLRenderPipelineDescriptor alloc] init];
		descriptor.vertexFunction = vertex_func;
		descriptor.fragmentFunction = fragment_func;
		descriptor.label = @"Growl shader";
		descriptor.colorAttachments[0].pixelFormat = dstTexture.pixelFormat;
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
	id<MTLRenderPipelineState> pipelineState = [[encoder.device
		newRenderPipelineStateWithDescriptor:descriptor
									   error:&error] autorelease];
	assert(!error);
	[error release];
	[encoder setRenderPipelineState:pipelineState];
}

NSString* const MetalShader::DEFAULT_SHADER = @R"(
#include <metal_stdlib>
using namespace metal;

struct ConstantBlock {
	float4x4 mvp;
};

struct VertexIn {
	float2 position;
	float2 vertPos;
};

struct VertexOut {
	float2 texCoord0;
	float4 position [[ position ]];
};

vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.position = constant_block.mvp * float4(v.position, 0, 1);

	return outVertex;
}

fragment float4 pixel_func (
	VertexOut v [[ stage_in ]],
	texture2d<float> tex0 [[ texture(0) ]],
	sampler sampler0 [[ sampler(0) ]]
) {
	return tex0.sample(sampler0, v.texCoord0);
}
)";
