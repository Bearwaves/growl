#pragma once

#include "growl/core/graphics/window.h"
#include <UIKit/UIKit.h>

namespace Growl {

class IOSWindow final : public Window {
public:
	explicit IOSWindow(UIWindow* window)
		: native{window} {}
	void* getNative() const override {
		return native;
	}

private:
	UIWindow* native;
};

} // namespace Growl
