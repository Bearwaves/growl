#include <growl/core/api/api.h>
#include <growl/plugins/metal/metal_graphics.h>

using Growl::API;
using Growl::MetalGraphicsAPI;

void initMetalPlugin(API& api) {
	api.addGraphicsAPI(std::make_unique<MetalGraphicsAPI>(*api.system()));
}
