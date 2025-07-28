#pragma once

#include "growl/core/error.h"
namespace Growl {

struct WindowSafeAreaInsets {
	float top;
	float bottom;
	float left;
	float right;
};

class Window {
public:
	virtual ~Window() = default;
	virtual void flip() {}
	virtual void getSize(int* w, int* h) {}
	virtual WindowSafeAreaInsets getSafeAreaInsets() {
		return WindowSafeAreaInsets{0, 0, 0, 0};
	}

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

	virtual void setFullscreen(bool fullscreen) {}
	virtual bool getFullscreen() {
		return true;
	}
	virtual void setVSync(bool vsync) {}
	virtual bool getVSync() {
		return true;
	}

#ifdef GROWL_IMGUI
	virtual void initImgui() {}
	virtual void newImguiFrame() {}
	virtual void populateDebugMenu() {}
#endif
};

} // namespace Growl
