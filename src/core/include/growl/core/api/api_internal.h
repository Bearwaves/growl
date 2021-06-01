#pragma once

#include "graphics_api.h"
#include "system_api.h"

namespace Growl {

class APIInternal {

	friend class API;

public:
	virtual ~APIInternal() {}

	virtual void init() = 0;
	virtual void dispose() = 0;
};

class SystemAPIInternal : public SystemAPI, public APIInternal {
public:
	virtual ~SystemAPIInternal() {}
};

class GraphicsAPIInternal : public GraphicsAPI, public APIInternal {
public:
	virtual ~GraphicsAPIInternal() {}
};

} // namespace Growl
