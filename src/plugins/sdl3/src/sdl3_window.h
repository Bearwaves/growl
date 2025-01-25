#pragma once

#include "SDL3/SDL_video.h"
#include "growl/core/graphics/window.h"

namespace Growl {

class SDL3Window final : public Window {
public:
	explicit SDL3Window(SDL_Window* window);
	~SDL3Window();

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
