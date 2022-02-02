#include "metal_batch.h"
#include "metal_texture.h"
#include <iostream>
#include <memory>

using Growl::MetalBatch;

void MetalBatch::begin() {
	encoder = [command_buffer
		renderCommandEncoderWithDescriptor:renderPassDescriptor()];
}

void MetalBatch::end() {
	[encoder endEncoding];
}

void MetalBatch::draw(Texture* texture, int x, int y, int width, int height) {
	auto tex = static_cast<MetalTexture*>(texture);
	shader->bind(surface, encoder);
	tex->bind(encoder);
	float fx = static_cast<float>(x);
	float fy = static_cast<float>(y);
	static float quadVertexData[] = {
		1, -1, 1.f, 1.f, -1, -1, 0.f, 1.f, -1, 1, 0.f, 0.f,

		1, -1, 1.f, 1.f, -1, 1,	 0.f, 0.f, 1,  1, 1.f, 0.f};

	id<MTLBuffer> buffer = [[encoder.device
		newBufferWithBytes:quadVertexData
					length:6 * 4 * 4
				   options:MTLResourceStorageModeShared] autorelease];
	[encoder setVertexBuffer:buffer offset:0 atIndex:0];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:6];
}

MTLRenderPassDescriptor* MetalBatch::renderPassDescriptor() {
	MTLRenderPassDescriptor* pass =
		[MTLRenderPassDescriptor renderPassDescriptor];
	pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
	pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	pass.colorAttachments[0].texture = surface;
	return pass;
}
