#pragma once

#include "api_internal.h"
#include "growl/core/frame_timer.h"

#include <memory>

namespace Growl {

class API {
public:
	void init();

	SystemAPI& system() {
		return *system_internal;
	}

	GraphicsAPI& graphics() {
		return *graphics_internal;
	}

	AudioAPI& audio() {
		return *audio_internal;
	}

	ScriptingAPI& scripting() {
		return *scripting_internal;
	}

	FrameTimer& frameTimer() {
		return *frame_timer;
	}

	bool imguiVisible() const {
		return imgui_visible;
	}

	void setImguiVisible(bool visible) {
		this->imgui_visible = visible;
	}

	void setSystemAPI(std::unique_ptr<SystemAPIInternal> system) {
		system_internal = std::move(system);
	}

	void setGraphicsAPI(std::unique_ptr<GraphicsAPIInternal> graphics) {
		graphics_internal = std::move(graphics);
	}

	void setAudioAPI(std::unique_ptr<AudioAPIInternal> audio) {
		audio_internal = std::move(audio);
	}

	void setScriptingAPI(std::unique_ptr<ScriptingAPIInternal> scripting) {
		scripting_internal = std::move(scripting);
	}

	void setFrameTimer(std::unique_ptr<FrameTimer> frame_timer) {
		this->frame_timer = std::move(frame_timer);
	}

private:
	std::unique_ptr<SystemAPIInternal> system_internal;
	std::unique_ptr<GraphicsAPIInternal> graphics_internal;
	std::unique_ptr<AudioAPIInternal> audio_internal;
	std::unique_ptr<ScriptingAPIInternal> scripting_internal;
	std::unique_ptr<FrameTimer> frame_timer;
	bool imgui_visible = false;
};

} // namespace Growl
