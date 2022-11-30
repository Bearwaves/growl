#ifdef GROWL_IMGUI

#include "imgui.h"
#include "growl/core/imgui.h"

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
}

void Growl::imGuiEnd() {
	ImGui::Render();
}

#endif
