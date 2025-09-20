#pragma once

#ifdef GROWL_IMGUI
typedef unsigned long long ImTextureID;
#endif

namespace Growl {

struct TextureOptions {
	bool filtering = true;
	bool mipmapped = true;
};

class Texture {
public:
	Texture(int width, int height)
		: width{width}
		, height{height} {}
	virtual ~Texture() = default;

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

#ifdef GROWL_IMGUI
	virtual ImTextureID getImguiTextureID() = 0;
#endif

protected:
	int width;
	int height;
};

} // namespace Growl
