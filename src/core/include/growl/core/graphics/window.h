#pragma once

#include "growl/core/error.h"
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
	virtual void flip(){};
	virtual void getSize(int* w, int* h){};

	virtual void* getMetalLayer() {
		return nullptr;
	}
	virtual Error createGLContext(int major_version, int minor_version, bool es = false) {
		return nullptr;
	}
	virtual void* (*glLibraryLoaderFunc(void))(const char*) {
		return nullptr;
	}
#ifdef GROWL_IMGUI
	virtual void initImgui() {}
	virtual void newImguiFrame() {}
#endif
};

} // namespace Growl
