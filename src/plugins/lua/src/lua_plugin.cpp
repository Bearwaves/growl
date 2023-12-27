#include "growl/core/api/api.h"
#include "lua_scripting.h"

using Growl::API;
using Growl::LuaScriptingAPI;

void initLuaPlugin(API& api) {
	api.addScriptingAPI(std::make_unique<LuaScriptingAPI>());
}
