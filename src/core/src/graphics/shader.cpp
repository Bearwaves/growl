#ifdef GROWL_IMGUI
#include "growl/core/graphics/shader.h"
#include "imgui.h"
#include "imgui_stdlib.h"

using Growl::Shader;

void Shader::populateDebugUI() {
	if (ImGui::Button("Recompile")) {
		if (auto err = compile(); err) {
			compile_error = err->message();
		} else {
			compile_error.clear();
		}
	}
	if (!compile_error.empty()) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Compile error");
		ImGui::InputTextMultiline(
			"##error", &compile_error, ImVec2(-1, 0),
			ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap);
	}
	ImGui::SeparatorText("Sources");
	ImGui::Text("Fragment");
	ImGui::InputTextMultiline(
		"##fragment", &(getFragmentSource()), ImVec2(-1, 0),
		ImGuiInputTextFlags_AllowTabInput);
	ImGui::Text("Vertex");
	ImGui::InputTextMultiline(
		"##vertex", &(getVertexSource()), ImVec2(-1, 0),
		ImGuiInputTextFlags_AllowTabInput);
	ImGui::Text("Uniforms");
	ImGui::InputTextMultiline(
		"##uniforms", &(getUniformsSource()), ImVec2(-1, 0),
		ImGuiInputTextFlags_AllowTabInput);
}

#endif
