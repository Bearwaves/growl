#pragma once

#include "growl/core/api/api_internal.h"
#include "growl/core/api/system_api.h"
#include "opengl_shader.h"
#include <SDL.h>
#include <chrono>

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
	Error setWindow(const WindowConfig& window_config) override;
	void clear(float r, float g, float b) override;

	std::unique_ptr<Texture>
	createTexture(const Image& image, const TextureOptions options) override;
	std::unique_ptr<Texture> createTexture(
		unsigned int width, unsigned int height,
		const TextureOptions options) override;

	std::unique_ptr<TextureAtlas> createTextureAtlas(
		const Atlas& atlas, const TextureOptions options) override;

	std::unique_ptr<FontTextureAtlas> createFontTextureAtlas(
		const FontAtlas& atlas, const TextureOptions options) override;

	std::unique_ptr<Batch> createBatch() override;
	std::unique_ptr<Batch> createBatch(const Texture& texture) override;

	void checkGLError(const char* file, long line);
	void checkShaderCompileError(unsigned int shader);

private:
	SystemAPI* system;
	std::unique_ptr<Window> window;
	std::unique_ptr<OpenGLShader> default_shader;
	SDL_GLContext context = nullptr;
	time_point<high_resolution_clock> last_render;

	std::unique_ptr<Texture> setupTexture(
		unsigned int texture_id, int width, int height,
		const TextureOptions options);

	void setupDebugCallback();
	void onGLDebugMessage(
		unsigned int source, unsigned int type, unsigned int id,
		unsigned int severity, const char* message) const;
};

} // namespace Growl
