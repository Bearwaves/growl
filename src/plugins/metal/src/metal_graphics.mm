#include <growl/plugins/metal/metal_graphics.h>

using Growl::MetalGraphicsAPI;

MetalGraphicsAPI::MetalGraphicsAPI(SystemAPI& system)
	: system{system} {}

void MetalGraphicsAPI::init() {}

void MetalGraphicsAPI::dispose() {}

void MetalGraphicsAPI::setWindow(WindowConfig& config) {
	window = system.createWindow(config);
}

void MetalGraphicsAPI::clear(float r, float g, float b) {}
