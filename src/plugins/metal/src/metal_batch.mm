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
	float halfWidth = surface.width / 2.f;
	float halfHeight = surface.height / 2.f;
	float left = (fx - halfWidth) / halfWidth;
	float right = left + (width / halfWidth);
	float top = -(fy - halfHeight) / halfHeight;
	float bottom = top - (height / halfHeight);
	float quadVertexData[] = {right, bottom, 1.f, 1.f, left,  bottom, 0.f, 1.f,
							  left,	 top,	 0.f, 0.f, right, bottom, 1.f, 1.f,
							  left,	 top,	 0.f, 0.f, right, top,	  1.f, 0.f};
	[encoder setVertexBytes:quadVertexData
					 length:6 * 4 * sizeof(float)
					atIndex:0];
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
