#pragma once

#include "growl/core/i18n/i18n.h"

namespace rapidcsv {
class Document;
}

namespace Growl {

class I18nCSVBundle : public I18nBundle {
public:
	I18nCSVBundle(
		rapidcsv::Document&& csv_document,
		I18nLocale default_locale = I18nLocale{"en", "", ""});

	I18nCSVBundle(
		rapidcsv::Document&& csv_document, std::string developer_relative_path,
		I18nLocale default_locale = I18nLocale{"en", "", ""});

	~I18nCSVBundle();

	// I18nCSVBundle is move-only
	I18nCSVBundle(const I18nCSVBundle&) = delete;
	I18nCSVBundle& operator=(const I18nCSVBundle&) = delete;
	I18nCSVBundle(I18nCSVBundle&&);
	I18nCSVBundle& operator=(I18nCSVBundle&&);

	Error reload() override;
	Error save() override;

#ifdef GROWL_IMGUI
	void populateDebugUI() override;
#endif

private:
	std::unique_ptr<rapidcsv::Document> csv_document;
	std::string developer_relative_path = "";
	std::string error_str = "";
};

} // namespace Growl
