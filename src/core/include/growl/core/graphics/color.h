#pragma once

namespace Growl {
struct Color {
	float r;
	float g;
	float b;
	float a;

	float* data() {
		return &r;
	}
};
} // namespace Growl
