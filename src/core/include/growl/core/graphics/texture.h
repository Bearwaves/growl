#pragma once

namespace Growl {

class Texture {
public:
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
