#include "shader_defaults.h"
#include <string>

using Growl::ShaderDefaults;

// OpenGL

const std::string ShaderDefaults::fragment_glsl = R"(in vec4 Color;
out vec4 outCol;

void main() {
	outCol = Color;
}
)";

const std::string ShaderDefaults::vertex_glsl = R"(void main() {
	TexCoord = texCoord;
	Dimensions = dimensions;
	Color = color;
	Idx = idx;
	gl_Position = projection * transforms[idx] * vec4(position, 0, 1);
}
)";

const std::string ShaderDefaults::uniforms_glsl = R"(struct Uniforms {
	float _padding;
};
)";

// Metal

const std::string ShaderDefaults::fragment_metal =
	R"(fragment float4 fragment_func (
	VertexOut v [[ stage_in ]]
) {
	return v.color;
}
)";

const std::string ShaderDefaults::vertex_metal =
	R"(vertex VertexOut vertex_func (
	constant ConstantBlock& constant_block [[ buffer(0) ]],
	const device VertexIn* vertex_array [[ buffer(1) ]],
	constant float4x4& transform [[ buffer(2) ]],
	unsigned int vid [[ vertex_id ]]
) {
	VertexIn v = vertex_array[vid];

	VertexOut outVertex = VertexOut();
	outVertex.texCoord0 = v.vertPos;
	outVertex.dimensions = v.dimensions;
	outVertex.position = constant_block.projection * transform * float4(v.position, 0, 1);
	outVertex.color = v.color;

	return outVertex;
}
)";

const std::string ShaderDefaults::uniforms_metal = "struct Uniforms {};";
