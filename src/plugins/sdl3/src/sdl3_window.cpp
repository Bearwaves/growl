#include "sdl3_window.h"
#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "growl/core/error.h"
#ifdef GROWL_IMGUI
#include "imgui_impl_sdl3.h"
#endif

using Growl::Error;
using Growl::SDL3Window;

SDL3Window::SDL3Window(SDL_Window* window)
	: native{window} {}

SDL3Window::~SDL3Window() {
	if (gl_context) {
		SDL_GL_DestroyContext(gl_context);
	}
	SDL_DestroyWindow(native);
}

void SDL3Window::flip() {
	SDL_GL_SwapWindow(native);
}

void SDL3Window::getSize(int* w, int* h) {
	SDL_GetWindowSizeInPixels(native, w, h);
}

void* SDL3Window::getMetalLayer() {
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
	SDL_Renderer* renderer =
		SDL_CreateRenderer(native, "metal");
	SDL_SetRenderVSync(renderer, 1);
	void* layer = SDL_GetRenderMetalLayer(renderer);
	SDL_DestroyRenderer(renderer);
	return layer;
}

Error SDL3Window::createGLContext(
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

void* (*SDL3Window::glLibraryLoaderFunc(void))(const char*) {
	return SDL_GL_GetProcAddress;
}

#ifdef GROWL_IMGUI
void SDL3Window::initImgui() {
	// Works for Metal too
	ImGui_ImplSDL3_InitForOpenGL(native, gl_context);
}

void SDL3Window::newImguiFrame() {
	ImGui_ImplSDL3_NewFrame();
}
#endif
