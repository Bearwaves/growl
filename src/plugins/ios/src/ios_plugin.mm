#include "growl/core/api/api.h"
#include "ios_system.h"
#include <memory>

using Growl::API;
using Growl::IOSSystemAPI;

void initIOSPlugin(API& api) {
	api.addSystemAPI(std::make_unique<IOSSystemAPI>(api));
}
