#include "sdl2_window.h"
#include "SDL_hints.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "growl/core/error.h"
#ifdef GROWL_IMGUI
#include "imgui_impl_sdl.h"
#endif

using Growl::Error;
using Growl::SDL2Window;

SDL2Window::SDL2Window(SDL_Window* window)
	: native{window} {}

SDL2Window::~SDL2Window() {
	if (gl_context) {
		SDL_GL_DeleteContext(gl_context);
	}
	SDL_DestroyWindow(native);
}

void SDL2Window::flip() {
	SDL_GL_SwapWindow(native);
}

void SDL2Window::getSize(int* w, int* h) {
	SDL_GL_GetDrawableSize(native, w, h);
}

void* SDL2Window::getMetalLayer() {
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
	SDL_Renderer* renderer =
		SDL_CreateRenderer(native, -1, SDL_RENDERER_PRESENTVSYNC);
	void* layer = SDL_RenderGetMetalLayer(renderer);
	SDL_DestroyRenderer(renderer);
	return layer;
}

Error SDL2Window::createGLContext(
	int major_version, int minor_version, bool es) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major_version);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor_version);
	if (!es) {
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	}
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		es ? SDL_GL_CONTEXT_PROFILE_ES : SDL_GL_CONTEXT_PROFILE_CORE);

	gl_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(native));
	return nullptr;
}

void* (*SDL2Window::glLibraryLoaderFunc(void))(const char*) {
	return SDL_GL_GetProcAddress;
}

#ifdef GROWL_IMGUI
void SDL2Window::initImgui() {
	// Works for Metal too
	ImGui_ImplSDL2_InitForOpenGL(native, gl_context);
}

void SDL2Window::newImguiFrame() {
	ImGui_ImplSDL2_NewFrame();
}
#endif
