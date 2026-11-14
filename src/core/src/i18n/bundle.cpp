#include "growl/core/i18n/i18n.h"

#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::Error;
using Growl::File;
using Growl::I18nBundle;
using Growl::I18nLocale;
using Growl::Result;

I18nBundle::I18nBundle(I18nLocale default_locale)
	: default_locale{default_locale} {}

I18nBundle::~I18nBundle() = default;

#ifdef GROWL_IMGUI
void I18nBundle::populateDebugUI() {
	if (ImGui::BeginTable("strings", 2, ImGuiTableFlags_Resizable)) {
		ImGui::TableSetupColumn("Key");
		ImGui::TableSetupColumn("Source");
		ImGui::TableHeadersRow();
		for (auto& key : keys) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", key.c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::TextWrapped("%s", source_strings[key].c_str());
		}
		ImGui::EndTable();
	}
}
#endif
