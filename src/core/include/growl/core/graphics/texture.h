#pragma once

namespace Growl {

class Texture {
public:
	Texture(int width, int height)
		: width{width}
		, height{height} {}

	// Texture is move-only
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;

	int getWidth() const {
		return width;
	}

	int getHeight() const {
		return height;
	}

protected:
	int width;
	int height;
};

} // namespace Growl
