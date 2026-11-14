#pragma once

#include "fmt/format.h"
#include "growl/core/error.h"
#include <map>
#include <string>

namespace Growl {

class File;

struct I18nLocale {
	std::string lang;
	std::string locale;
	std::string script;
};

class I18nBundle {
public:
	explicit I18nBundle(I18nLocale default_locale);
	virtual ~I18nBundle();

	virtual Error reload() = 0;
	virtual Error save() = 0;

	template <class... Args>
	std::string getString(const std::string& key, Args&&... args) {
		auto it = source_strings.find(key);
		return fmt::format(
			it == source_strings.end() ? key : it->second,
			std::forward<Args>(args)...);
	}

#ifdef GROWL_IMGUI
	virtual void populateDebugUI();
#endif

protected:
	I18nLocale default_locale;
	std::vector<std::string> keys;
	std::map<std::string_view, std::string> source_strings;
};

} // namespace Growl
