#pragma once

#include "growl/core/error.h"
#include "growl/core/graphics/window.h"
#include <EGL/egl.h>
#include <android/native_window.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Growl {

class AndroidWindow final : public Window {
public:
	explicit AndroidWindow(ANativeWindow* window, android_app* app)
		: app{app} {}
	~AndroidWindow();

	void flip() override;
	void getSize(int* w, int* h) override;

	WindowSafeAreaInsets getSafeAreaInsets() override;

	Error
	createGLContext(int major_version, int minor_version, bool es) override;

	Error initSurface();
	Error deinitSurface();

private:
	android_app* app;

	EGLDisplay display = EGL_NO_DISPLAY;
	EGLConfig egl_config = nullptr;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;
};

} // namespace Growl
