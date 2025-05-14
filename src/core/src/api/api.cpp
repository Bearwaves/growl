#include "growl/core/api/api_internal.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::SystemAPIInternal;

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
