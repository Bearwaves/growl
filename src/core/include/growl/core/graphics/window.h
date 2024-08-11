#pragma once

#include "growl/core/error.h"
namespace Growl {

class Window {
public:
	virtual ~Window() = default;
	virtual void flip() {};
	virtual void getSize(int* w, int* h) {};

	virtual void* getMetalLayer() {
		return nullptr;
	}
	virtual Error
	createGLContext(int major_version, int minor_version, bool es = false) {
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
