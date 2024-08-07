#pragma once

#include "api_internal.h"
#include "growl/core/frame_timer.h"

#include <memory>

namespace Growl {

class API {
public:
	SystemAPI& system() {
		return *systemInternal;
	}

	GraphicsAPI& graphics() {
		return *graphicsInternal;
	}

	AudioAPI& audio() {
		return *audioInternal;
	}

	ScriptingAPI& scripting() {
		return *scriptingInternal;
	}

	FrameTimer& frameTimer() {
		return *frame_timer;
	}

	void addSystemAPI(std::unique_ptr<SystemAPIInternal> internal);
	void addGraphicsAPI(std::unique_ptr<GraphicsAPIInternal> internal);
	void addAudioAPI(std::unique_ptr<AudioAPIInternal> internal);
	void addScriptingAPI(std::unique_ptr<ScriptingAPIInternal> internal);

	bool imguiVisible() const {
		return imgui_visible;
	}

	void setImguiVisible(bool visible) {
		this->imgui_visible = visible;
	}

	void setSystemAPI(std::unique_ptr<SystemAPIInternal> system) {
		systemInternal = std::move(system);
	}

	void setGraphicsAPI(std::unique_ptr<GraphicsAPIInternal> graphics) {
		graphicsInternal = std::move(graphics);
	}

	void setAudioAPI(std::unique_ptr<AudioAPIInternal> audio) {
		audioInternal = std::move(audio);
	}

	void setScriptingAPI(std::unique_ptr<ScriptingAPIInternal> scripting) {
		scriptingInternal = std::move(scripting);
	}

	void setFrameTimer(std::unique_ptr<FrameTimer> frame_timer) {
		this->frame_timer = std::move(frame_timer);
	}

private:
	std::unique_ptr<SystemAPIInternal> systemInternal;
	std::unique_ptr<GraphicsAPIInternal> graphicsInternal;
	std::unique_ptr<AudioAPIInternal> audioInternal;
	std::unique_ptr<ScriptingAPIInternal> scriptingInternal;
	std::unique_ptr<FrameTimer> frame_timer;
	bool imgui_visible = false;
};

} // namespace Growl
