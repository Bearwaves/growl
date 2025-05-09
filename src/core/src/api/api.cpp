#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::API;
using Growl::SystemAPIInternal;

void API::init() {
	if (!this->system_internal) {
		this->system_internal = createSystemAPI(*this);
	}
	if (!this->graphics_internal) {
		this->graphics_internal = createGraphicsAPI(*this);
	}
	if (!this->audio_internal) {
		this->audio_internal = createAudioAPI(*this);
	}
	if (!this->scripting_internal) {
		this->scripting_internal = createScriptingAPI(*this);
	}
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
