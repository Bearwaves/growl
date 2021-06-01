#include <growl/plugins/dummy/dummy_graphics.h>

#include <iostream>

using namespace Growl;

void DummyGraphicsAPI::create() {}

void DummyGraphicsAPI::dispose() {}

void DummyGraphicsAPI::clear(float r, float g, float b) {
	std::cout << "Cleared screen with: (" << r << ", " << g << ", " << b << ")."
			  << std::endl;
}
