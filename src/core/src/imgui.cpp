#ifdef GROWL_IMGUI

#include "growl/core/imgui.h"
#include "imgui.h"

void Growl::imGuiBegin() {
	ImGui::NewFrame();
	ImGui::BeginMainMenuBar();
	ImGui::Text("Growl");
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
	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockSpaceOverViewport(dockspace_id);
}

void Growl::imGuiBeginGameWindow() {
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::BeginChild(
		"GameContent", ImVec2{0, 0}, false, ImGuiWindowFlags_NoMove);
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
	ImGui::EndChild();
	ImGui::End();
}

bool Growl::imGuiGameWindowFocused() {
	return game_window_focused;
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
}

#endif
