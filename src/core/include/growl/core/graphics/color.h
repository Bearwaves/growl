#pragma once

namespace Growl {
struct Color {
	float r;
	float g;
	float b;
	float a;

	Color()
		: Color(0, 0, 0, 0) {}

	Color(float r, float g, float b, float a)
		: r{r}
		, g{g}
		, b{b}
		, a{a} {}

	Color(float r, float g, float b)
		: Color{r, g, b, 1.f} {}

	Color(unsigned int hex_value) {
		r = ((hex_value >> 16) & 0xFF) / 255.0;
		g = ((hex_value >> 8) & 0xFF) / 255.0;
		b = ((hex_value) & 0xFF) / 255.0;
		a = 1;
	}

	float* data() {
		return &r;
	}
};
} // namespace Growl
