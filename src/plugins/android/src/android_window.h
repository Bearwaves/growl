#pragma once

#include "growl/core/error.h"
#include "growl/core/graphics/window.h"
#include <EGL/egl.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>

namespace Growl {

class AndroidWindow final : public Window {
public:
	explicit AndroidWindow(ANativeWindow* window, android_app* app)
		: native{window}
		, app{app} {}
	~AndroidWindow();

	void flip() override;
	void getSize(int* w, int* h) override;

	WindowSafeAreaInsets getSafeAreaInsets() override;

	Error
	createGLContext(int major_version, int minor_version, bool es) override;

private:
	ANativeWindow* native;
	android_app* app;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

} // namespace Growl
