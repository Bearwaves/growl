#include "android_system.h"
#include "growl/core/api/api.h"
#include <memory>

using Growl::AndroidSystemAPI;
using Growl::API;

void initAndroidPlugin(API& api, android_app* state) {
	api.addSystemAPI(std::make_unique<AndroidSystemAPI>(api, state));
}
