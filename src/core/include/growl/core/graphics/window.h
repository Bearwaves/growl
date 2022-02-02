#pragma once

#include <string>
namespace Growl {

class WindowConfig {
public:
	WindowConfig(std::string title, int width, int height, bool centred = true)
		: title{title}
		, width{width}
		, height{height}
		, centred{centred} {}
	std::string getTitle() const {
		return title;
	}
	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	bool isCentred() const {
		return centred;
	}

private:
	std::string title;
	int width;
	int height;
	bool centred;
};

class Window {
public:
	virtual ~Window() = default;
	virtual void* getNative() const = 0;
};

} // namespace Growl
