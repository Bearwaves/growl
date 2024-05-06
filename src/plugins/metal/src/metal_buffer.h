#pragma once

#include <Metal/Metal.h>
#include <cstring>

namespace Growl {

enum class BufferBinding { Vertex, Fragment, Both };

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
		size_t length, BufferBinding binding = BufferBinding::Vertex) {
		std::memcpy(
			static_cast<unsigned char*>(buffer.contents) + *offset, from,
			length);
		if (binding == BufferBinding::Vertex ||
			binding == BufferBinding::Both) {
			[encoder setVertexBuffer:buffer offset:*offset atIndex:index];
		}
		if (binding == BufferBinding::Fragment ||
			binding == BufferBinding::Both) {
			[encoder setFragmentBuffer:buffer offset:*offset atIndex:index];
		}
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
