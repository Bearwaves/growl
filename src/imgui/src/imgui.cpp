#ifdef GROWL_IMGUI
#include "growl/imgui/imgui.h"
#include "growl/core/api/api.h"
#include "growl/core/graphics/window.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <set>

constexpr const char* SYSTEM_API_WINDOW = "System API";
constexpr const char* WINDOW_WINDOW = "Window";

static bool game_window_focused;
static int game_window_x;
static int game_window_y;
static int game_window_w = 0;
static int game_window_h = 0;
static bool game_window_resized;
static bool system_api_view = false;
static bool window_view = false;
static bool needs_setup = true;
static bool first_render = true;
static ImGuiID dockspace_id = 0;
static ImGuiID dockspace_left = 0;
static ImGuiID dockspace_right = 0;
static ImGuiID dockspace_bottom = 0;

static std::set<std::string> open_windows;
static ImGuiSettingsHandler handler;

namespace Growl {
void doApiWindows(API& api) {
	if (system_api_view) {
		ImGui::Begin(SYSTEM_API_WINDOW, &system_api_view);
		static_cast<SystemAPIInternal&>(api.system()).populateDebugMenu();
		ImGui::End();
		if (needs_setup) {
			ImGui::DockBuilderDockWindow(SYSTEM_API_WINDOW, dockspace_right);
		}
		open_windows.insert(SYSTEM_API_WINDOW);
	} else {
		open_windows.erase(SYSTEM_API_WINDOW);
	}
	if (window_view) {
		ImGui::Begin(WINDOW_WINDOW, &window_view);
		static_cast<GraphicsAPIInternal&>(api.graphics())
			.getWindow()
			->populateDebugMenu();
		ImGui::End();
		if (needs_setup) {
			ImGui::DockBuilderDockWindow(WINDOW_WINDOW, dockspace_bottom);
		}
		open_windows.insert(WINDOW_WINDOW);
	} else {
		open_windows.erase(WINDOW_WINDOW);
	}
}
} // namespace Growl

void imGuiSetupIni();

void Growl::imGuiSetup() {
	imGuiSetupIni();
}

void Growl::imGuiBegin(API& api) {
	ImGui::NewFrame();
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Growl")) {
		if (ImGui::MenuItem("Reset debug UI")) {
			needs_setup = true;
			ImGui::DockBuilderRemoveNode(dockspace_id);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Views")) {
		ImGui::SeparatorText("Growl APIs");
		ImGui::MenuItem("System API", nullptr, &system_api_view);
		ImGui::MenuItem("Window", nullptr, &window_view);
		ImGui::EndMenu();
	}
	auto size = ImGui::CalcTextSize("0.00 ms/frame (000.0 FPS)");
	ImGuiStyle& style = ImGui::GetStyle();
	size.x += style.FramePadding.x * 2 + style.ItemSpacing.x;
	auto cursor = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(ImGui::GetIO().DisplaySize.x - size.x);
	ImGui::Text(
		"%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
	ImGui::SetCursorPosX(cursor);
	ImGui::EndMainMenuBar();
	dockspace_id = ImGui::GetID("DockSpace");
	if (needs_setup) {
		needs_setup = !ImGui::DockBuilderGetNode(dockspace_id);
		ImGui::DockSpaceOverViewport(dockspace_id);
		if (needs_setup) {
			api.system().log("ImGui", "Dock space needs first time setup");
			ImGui::DockBuilderDockWindow("Game", dockspace_id);
			ImGuiID main;
			dockspace_left = ImGui::DockBuilderSplitNode(
				dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &main);
			dockspace_right = ImGui::DockBuilderSplitNode(
				dockspace_id, ImGuiDir_Right, 0.2f, nullptr, nullptr);
			dockspace_bottom = ImGui::DockBuilderSplitNode(
				main, ImGuiDir_Down, 0.2f, nullptr, nullptr);
			ImGui::DockBuilderFinish(dockspace_id);
		}
	} else {
		ImGui::DockSpaceOverViewport(dockspace_id);
	}
	doApiWindows(api);
}

void Growl::imGuiBeginGameWindow() {
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse);
	game_window_focused = ImGui::IsWindowFocused();
	auto pos = ImGui::GetCursorScreenPos();
	game_window_x = pos.x * ImGui::GetIO().DisplayFramebufferScale.x;
	game_window_y = pos.y * ImGui::GetIO().DisplayFramebufferScale.y;
	auto window_size = ImGui::GetContentRegionAvail();
	int new_game_window_w =
		window_size.x * ImGui::GetIO().DisplayFramebufferScale.x;
	int new_game_window_h =
		window_size.y * ImGui::GetIO().DisplayFramebufferScale.y;
	if (new_game_window_w != game_window_w ||
		new_game_window_h != game_window_h) {
		game_window_w = new_game_window_w;
		game_window_h = new_game_window_h;
		game_window_resized = true;
	}
}

void Growl::imGuiEndGameWindow() {
	ImGui::End();
	if (first_render) {
		ImGui::SetWindowFocus("Game");
		first_render = false;
	}
}

bool Growl::imGuiGameWindowFocused() {
	return game_window_focused;
}

void Growl::imGuiFocusGameWindow() {
	ImGui::SetWindowFocus("Game");
	game_window_focused = true;
}

void Growl::imGuiGameWindowSize(int* w, int* h) {
	*w = game_window_w;
	*h = game_window_h;
}

void Growl::imGuiGameWindowPos(int* x, int* y) {
	*x = game_window_x;
	*y = game_window_y;
}

// Not thread safe. Can only be used once per frame.
bool Growl::imGuiGameWindowResized() {
	bool did_resize = game_window_resized;
	game_window_resized = false;
	return did_resize;
}

void Growl::imGuiEnd() {
	ImGui::Render();
	if (needs_setup) {
		needs_setup = false;
	}
}

bool Growl::imGuiWindow(
	const char* name, bool default_open,
	ImGuiDockDirection default_dock_direction) {
	bool open =
		ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
	if (needs_setup) {
		switch (default_dock_direction) {
		case ImGuiDockDirection::Left:
			imGuiDockLeft(name);
			break;
		case ImGuiDockDirection::Right:
			imGuiDockRight(name);
			break;
		case ImGuiDockDirection::Bottom:
			imGuiDockBottom(name);
			break;
		default:
			break;
		}
	}
	return open;
}

void Growl::imGuiDockLeft(const char* name) {
	if (needs_setup) {
		ImGui::DockBuilderDockWindow(name, dockspace_left);
	}
}

void Growl::imGuiDockRight(const char* name) {
	if (needs_setup) {
		ImGui::DockBuilderDockWindow(name, dockspace_right);
	}
}

void Growl::imGuiDockBottom(const char* name) {
	if (needs_setup) {
		ImGui::DockBuilderDockWindow(name, dockspace_bottom);
	}
}

void imGuiSetupIni() {
	handler.TypeName = "Growl";
	handler.TypeHash = ImHashStr(handler.TypeName);

	auto clear_all = [](ImGuiContext*, ImGuiSettingsHandler*) -> void {
		open_windows.clear();
	};
	handler.ClearAllFn = clear_all;
	handler.ReadInitFn = clear_all;

	handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler*,
							const char* name) -> void* {
		if (std::string(name) != "Windows") {
			return nullptr;
		}
		return (void*)1;
	};

	handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry,
							const char* line) -> void {
		auto l = std::string(ImStrSkipBlank(line));
		size_t pos = l.find('=');
		if (pos == std::string::npos) {
			// No =
			return;
		}
		auto window = l.substr(0, pos);
		open_windows.insert(window);
		if (window == SYSTEM_API_WINDOW) {
			system_api_view = true;
		} else if (window == WINDOW_WINDOW) {
			window_view = true;
		}
	};

	handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler*,
							ImGuiTextBuffer* buf) -> void {
		buf->appendf("[%s][Windows]\n", handler.TypeName);
		for (auto& window : open_windows) {
			buf->appendf("%s=1\n", window.c_str());
		}
	};

	ImGui::AddSettingsHandler(&handler);
}

#endif
