#ifdef GROWL_IMGUI
#include "growl/core/graphics/shader.h"
#include "growl/core/assets/shader_pack.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <filesystem>
#include <fstream>
#include <sstream>

using Growl::Shader;
using Growl::ShaderPack;
using Growl::ShaderType;

void Shader::populateDebugUI() {
	if (!compile_error.empty()) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Compile error");
		ImGui::InputTextMultiline(
			"##error", &compile_error, ImVec2(-1, 0),
			ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap);
	}
	ImGui::SeparatorText("Sources");
	if (ImGui::Button("Recompile")) {
		if (auto err = compile(); err) {
			compile_error = err->message();
		} else {
			compile_error.clear();
		}
	}
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

std::string getExtension(ShaderType type) {
	switch (type) {
	case Growl::ShaderType::GLSL:
		return "glsl";
	case Growl::ShaderType::Metal:
		return "metal";
	default:
		return "";
	}
}

std::string reloadFromFile(
	std::filesystem::path p, std::string source_type, ShaderType shader_type) {
	std::ifstream f;
	p = std::filesystem::canonical(p) /
		(source_type + getExtension(shader_type));
	f.open(p, std::ios::in);
	if (f.fail()) {
		return "ERROR loading file:\n" + p.string();
	}
	std::ostringstream stream;
	stream << f.rdbuf();
	return stream.str();
}

void Shader::populateDebugUI(ShaderPack& pack) {
	if (ImGui::TreeNodeEx(pack.getName().c_str(), ImGuiTreeNodeFlags_Framed)) {
		if (!pack.getDeveloperRelativePath().empty()) {
			ImGui::Text(
				"Source path: %s", pack.getDeveloperRelativePath().c_str());
			if (ImGui::Button("Reload") && pack.getSources().count(getType())) {
				auto& source = pack.getSources().at(getType());
				if (source.fragment_src.has_value()) {
					getFragmentSource() = reloadFromFile(
						pack.getDeveloperRelativePath(), "fragment.",
						getType());
				}
				if (source.vertex_src.has_value()) {
					getVertexSource() = reloadFromFile(
						pack.getDeveloperRelativePath(), "vertex.", getType());
				}
				if (source.uniforms_src.has_value()) {
					getFragmentSource() = reloadFromFile(
						pack.getDeveloperRelativePath(), "uniforms.",
						getType());
				}
				if (auto err = compile(); err) {
					compile_error = err->message();
				} else {
					compile_error.clear();
				}
			}
		}
		populateDebugUI();
		ImGui::TreePop();
	}
}

#endif
