#pragma once

#include "audio_api.h"
#include "graphics_api.h"
#include "growl/core/error.h"
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
};

class GraphicsAPIInternal : public GraphicsAPI, public APIInternal {
public:
	virtual ~GraphicsAPIInternal() {}
	virtual Error setWindow(const WindowConfig& window_descriptor) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
};

class AudioAPIInternal : public AudioAPI, public APIInternal {
public:
	virtual ~AudioAPIInternal() {}
};

} // namespace Growl
