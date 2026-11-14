#include "growl/core/i18n/i18n_csv_bundle.h"

#include "growl/core/assets/error.h"
#include "growl/core/error.h"
#include "growl/core/i18n/i18n.h"
#include "rapidcsv.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::Error;
using Growl::I18nBundle;
using Growl::I18nCSVBundle;
using Growl::I18nLocale;

I18nCSVBundle::I18nCSVBundle(
	rapidcsv::Document&& csv_document, std::string developer_relative_path,
	I18nLocale default_locale)
	: I18nBundle{default_locale}
	, csv_document{std::make_unique<rapidcsv::Document>(
		  std::move(csv_document))}
	, developer_relative_path{developer_relative_path} {}

I18nCSVBundle::I18nCSVBundle(
	rapidcsv::Document&& csv_document, I18nLocale default_locale)
	: I18nBundle{default_locale}
	, csv_document{
		  std::make_unique<rapidcsv::Document>(std::move(csv_document))} {}

I18nCSVBundle::~I18nCSVBundle() = default;
I18nCSVBundle::I18nCSVBundle(I18nCSVBundle&&) = default;
I18nCSVBundle& I18nCSVBundle::operator=(I18nCSVBundle&&) = default;

std::string replaceStr(
	const std::string& str, const std::string& from, const std::string& to) {
	std::string out = str;
	size_t start_pos = 0;
	while ((start_pos = out.find(from, start_pos)) != std::string::npos) {
		out.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return out;
}

Error I18nCSVBundle::reload() {
	if (csv_document->GetColumnIdx("key") == -1 ||
		csv_document->GetColumnIdx("source") == -1) {
		return Error(
			std::make_unique<AssetsError>(
				"CSV document requires 'key' and 'source' columns"));
	}
	this->keys = csv_document->GetColumn<std::string>("key");
	for (size_t i = 0; i < this->keys.size(); i++) {
		this->source_strings[keys[i]] = csv_document->GetCell<std::string>(
			"source", i, [](const std::string& from, std::string& to) -> void {
				to = replaceStr(from, "\\n", "\n");
			});
	}
	return nullptr;
}

Error I18nCSVBundle::save() {
	if (developer_relative_path.empty()) {
		return nullptr;
	}
	try {
		csv_document->Clear();
		csv_document->InsertRow(-1, std::vector<std::string>{"key", "source"});
		int idx = 0;
		for (auto& key : keys) {
			csv_document->InsertRow(
				idx++, std::vector<std::string>{
						   key, replaceStr(source_strings[key], "\n", "\\n")});
		}
		csv_document->Save(developer_relative_path);
	} catch (std::exception& e) {
		error_str = "Failed to save: ";
		error_str += e.what();
	}

	return nullptr;
}

#ifdef GROWL_IMGUI
void I18nCSVBundle::populateDebugUI() {
	if (!developer_relative_path.empty()) {
		if (ImGui::Button("Reload from file")) {
			error_str = "";
			try {
				keys.clear();
				source_strings.clear();
				csv_document->Load(developer_relative_path);
				if (auto err = reload()) {
					error_str = "Failed to reload: ";
					error_str += err->message();
				}
			} catch (std::exception& e) {
				error_str = "Failed to reload: ";
				error_str += e.what();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Save to file")) {
			error_str = "";
			save();
		}
	}
	if (!error_str.empty()) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", error_str.c_str());
	}
	I18nBundle::populateDebugUI();
}
#endif
