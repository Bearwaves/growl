#pragma once

#include "SDL3/SDL_video.h"
#include "growl/core/graphics/window.h"

namespace Growl {

class SystemAPI;

class SDL3Window final : public Window {
public:
	explicit SDL3Window(SDL_Window* window, SystemAPI& system);
	~SDL3Window();

	void flip() override;
	void getSize(int* w, int* h) override;

	void* getMetalLayer() override;
	Error
	createGLContext(int major_version, int minor_version, bool es) override;
	void* (*glLibraryLoaderFunc(void))(const char*) override;

	void setFullscreen(bool fullscreen) override;
	bool getFullscreen() override;
	void setVSync(bool vsync) override;
	bool getVSync() override;

#ifdef GROWL_IMGUI
	void initImgui() override;
	void newImguiFrame() override;
	void populateDebugMenu() override;
	WindowSafeAreaInsets getSafeAreaInsets() override {
		return insets;
	}
#endif

private:
	SDL_Window* native;
	SystemAPI& system;

	// OpenGL
	SDL_GLContext gl_context = nullptr;

#ifdef GROWL_IMGUI
	WindowSafeAreaInsets insets{0, 0, 0, 0};
#endif
};

} // namespace Growl
