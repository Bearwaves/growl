#pragma once

#include "growl/core/error.h"
#include "growl/core/graphics/window.h"
#include <EGL/egl.h>
#include <android/native_window.h>

namespace Growl {

class AndroidWindow final : public Window {
public:
	explicit AndroidWindow(ANativeWindow* window)
		: native{window} {}
	~AndroidWindow();

	void flip() override;
	void getSize(int* w, int* h) override;

	Error createGLContext(int major_version, int minor_version, bool es) override;

private:
	ANativeWindow* native;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

} // namespace Growl
