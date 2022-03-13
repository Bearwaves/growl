#pragma once

#include "graphics_api.h"
#include "system_api.h"
#include <growl/util/error.h>

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
	virtual void setWindow(WindowConfig& windowDescriptor) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
};

} // namespace Growl
