#include "android_window.h"
#include "android_error.h"
#include "growl/core/error.h"
#include <EGL/egl.h>

using Growl::AndroidError;
using Growl::AndroidWindow;
using Growl::Error;

AndroidWindow::~AndroidWindow() {
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
}

void AndroidWindow::flip() {
	eglSwapBuffers(display, surface);
}

void AndroidWindow::getSize(int* w, int* h) {
	eglQuerySurface(display, surface, EGL_WIDTH, w);
	eglQuerySurface(display, surface, EGL_HEIGHT, h);
}

Error AndroidWindow::createGLContext(
	int major_version, int minor_version, bool es) {
	const EGLint attribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
							  EGL_BLUE_SIZE,	8,
							  EGL_GREEN_SIZE,	8,
							  EGL_RED_SIZE,		8,
							  EGL_NONE};
	EGLint num_configs;
	EGLConfig egl_config = nullptr;
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, nullptr, nullptr);

	// Choose the best config
	eglChooseConfig(display, attribs, nullptr, 0, &num_configs);
	std::unique_ptr<EGLConfig[]> supported_configs(new EGLConfig[num_configs]);
	eglChooseConfig(
		display, attribs, supported_configs.get(), num_configs, &num_configs);
	if (!num_configs) {
		return std::make_unique<AndroidError>("Failed to find EGL configs");
	}
	auto i = 0;
	for (; i < num_configs; i++) {
		auto& cfg = supported_configs[i];
		EGLint r, g, b, d;
		if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
			eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
			eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
			eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && r == 8 &&
			g == 8 && b == 8 && d == 0) {

			egl_config = supported_configs[i];
			break;
		}
	}
	if (i == num_configs) {
		egl_config = supported_configs[0];
	}

	// todo handle error
	surface = eglCreateWindowSurface(display, egl_config, native, nullptr);
	const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
	context = eglCreateContext(display, egl_config, nullptr, context_attribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		return std::make_unique<AndroidError>(
			"Failed to get an EGL display: code " +
			std::to_string(eglGetError()));
	}

	return nullptr;
}
