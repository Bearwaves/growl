#pragma once

#include "audio_api.h"
#include "graphics_api.h"
#include "growl/core/error.h"
#include "growl/core/input/event.h"
#include "scripting_api.h"
#include "system_api.h"

namespace Growl {

class APIInternal {

	friend class API;

public:
	virtual ~APIInternal() {}

	virtual Error init() = 0;
	virtual void dispose() = 0;
};

class SystemAPIInternal : public SystemAPI, public APIInternal {
public:
	virtual ~SystemAPIInternal() {}
	// Used in push-style touch input e.g. on iOS.
	virtual void onTouch(InputTouchEvent touch) {}
};

class GraphicsAPIInternal : public GraphicsAPI, public APIInternal {
public:
	virtual ~GraphicsAPIInternal() {}
	virtual Error setWindow(const WindowConfig& window_descriptor) = 0;
	virtual void onWindowResize(int width, int height) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
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
