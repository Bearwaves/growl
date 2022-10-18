#pragma once

#include "growl/core/graphics/window.h"
#include <SDL.h>

namespace Growl {

class SDL2Window final : public Window {
public:
	explicit SDL2Window(SDL_Window* window);
	~SDL2Window();

	void flip() override;
	void getSize(int* w, int* h) override;

	void* getMetalLayer() override;
	Error
	createGLContext(int major_version, int minor_version, bool es) override;
	void* (*glLibraryLoaderFunc(void))(const char*) override;

#ifdef GROWL_IMGUI
	void initImgui() override;
	void newImguiFrame() override;
#endif

private:
	SDL_Window* native;

	// OpenGL
	SDL_GLContext gl_context = nullptr;
};

} // namespace Growl
