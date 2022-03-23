#pragma once

#include "opengl_shader.h"
#include <SDL.h>
#include <chrono>
#include <growl/core/api/api_internal.h>
#include <growl/core/api/system_api.h>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;

namespace Growl {
class OpenGLGraphicsAPI : public GraphicsAPIInternal {
public:
	explicit OpenGLGraphicsAPI(SystemAPI* system);
	Error init() override;
	void dispose() override;
	void begin() override;
	void end() override;
	Error setWindow(WindowConfig& windowConfig) override;
	void clear(float r, float g, float b) override;
	std::unique_ptr<Texture> createTexture(const Image& image) override;
	std::unique_ptr<Batch> createBatch() override;

	void checkGLError(const char* file, long line);
	void checkShaderCompileError(unsigned int shader);

private:
	SystemAPI* system;
	std::unique_ptr<Window> window;
	std::unique_ptr<OpenGLShader> default_shader;
	SDL_GLContext context;
	time_point<high_resolution_clock> last_render;

	void setupDebugCallback();
	void onGLDebugMessage(
		unsigned int source, unsigned int type, unsigned int id,
		unsigned int severity, const char* message) const;
};

} // namespace Growl
