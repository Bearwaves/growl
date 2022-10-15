#pragma once

#include "growl/core/graphics/window.h"
#include <android/native_window.h>

namespace Growl {

class AndroidWindow final : public Window {
public:
	explicit AndroidWindow(ANativeWindow* window)
		: native{window} {}
	void* getNative() const override {
		return native;
	}

private:
	ANativeWindow* native;
};

} // namespace Growl
