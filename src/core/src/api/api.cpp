#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::API;
using Growl::SystemAPIInternal;

void API::addSystemAPI(std::unique_ptr<SystemAPIInternal> internal) {
	systemInternal = std::move(internal);
}

void API::addGraphicsAPI(std::unique_ptr<GraphicsAPIInternal> internal) {
	graphicsInternal = std::move(internal);
}

void API::addAudioAPI(std::unique_ptr<AudioAPIInternal> internal) {
	audioInternal = std::move(internal);
}

void API::addScriptingAPI(std::unique_ptr<ScriptingAPIInternal> internal) {
	scriptingInternal = std::move(internal);
}

#ifdef GROWL_IMGUI
void SystemAPIInternal::populateDebugMenu() {
	bool dark_mode = isDarkMode();
	if (ImGui::Checkbox("Dark mode", &dark_mode)) {
		setDarkMode(dark_mode);
	}

	bool mouse_as_touch = isMouseEmitsTouchEvents();
	if (ImGui::Checkbox("Mouse emits touch events", &mouse_as_touch)) {
		setMouseEmitsTouchEvents(mouse_as_touch);
	}
}
#endif
