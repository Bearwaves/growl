#pragma once

#include "audio_api.h"
#include "graphics_api.h"
#include "growl/core/config.h"
#include "growl/core/error.h"
#include "growl/core/input/event.h"
#include "scripting_api.h"
#include "system_api.h"

namespace Growl {

class APIInternal {

	friend class API;

public:
	virtual ~APIInternal() {}

	virtual Error init(const Config& config) = 0;
	virtual void dispose() = 0;
#ifdef GROWL_IMGUI
	virtual void populateDebugMenu() {}
#endif
};

class SystemAPIInternal : public SystemAPI, public APIInternal {
public:
	virtual ~SystemAPIInternal() {}
	// Used in push-style touch input e.g. on iOS.
	virtual void onTouch(InputTouchEvent touch) {}
	virtual void setDarkMode(bool dark_mode) {
		this->dark_mode = dark_mode;
	}
	virtual void pause() {
		this->paused = true;
	}
	virtual void resume() {
		this->paused = false;
	}
	virtual bool isMouseEmitsTouchEvents() {
		return this->mouse_emits_touch_events;
	}
	virtual void setMouseEmitsTouchEvents(bool mouse_as_touch) {
		this->mouse_emits_touch_events = mouse_as_touch;
	}
#ifdef GROWL_IMGUI
	virtual void populateDebugMenu() override;
#endif
};

class GraphicsAPIInternal : public GraphicsAPI, public APIInternal {
public:
	virtual ~GraphicsAPIInternal() {}
	virtual Error setWindow(const Config& config) = 0;
	virtual void onWindowResize(int width, int height) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual Window* getWindow() = 0;
};

class AudioAPIInternal : public AudioAPI, public APIInternal {
public:
	virtual ~AudioAPIInternal() {}
};

class ScriptingAPIInternal : public ScriptingAPI, public APIInternal {
public:
	virtual ~ScriptingAPIInternal() {}
};

} // namespace Growl
