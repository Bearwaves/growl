#pragma once

#include <Metal/Metal.h>
#include <cstring>

namespace Growl {
class MetalBuffer {
public:
	MetalBuffer(id<MTLBuffer> buffer, size_t* offset)
		: buffer{buffer}
		, offset{offset} {}

	void write(void* from, size_t length) {
		std::memcpy(buffer.contents, from, length);
		*offset += length;
	}

	void writeAndBind(
		id<MTLRenderCommandEncoder> encoder, size_t index, void* from,
		size_t length) {
		std::memcpy(
			static_cast<unsigned char*>(buffer.contents) + *offset, from,
			length);
		[encoder setVertexBuffer:buffer offset:*offset atIndex:index];
		*offset += length;
	}

	id<MTLBuffer> getBuffer() {
		return this->buffer;
	}

private:
	id<MTLBuffer> buffer;
	size_t* offset;
};
} // namespace Growl
