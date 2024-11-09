#include "ios_window.h"

using Growl::IOSWindow;
using Growl::WindowSafeAreaInsets;

void* IOSWindow::getMetalLayer() {
	return native.rootViewController.view.layer;
}

WindowSafeAreaInsets IOSWindow::getSafeAreaInsets() {
	auto insets = native.safeAreaInsets;
	auto scale = native.screen.scale;
	return WindowSafeAreaInsets{
		static_cast<float>(insets.top * scale),
		static_cast<float>(insets.bottom * scale),
		static_cast<float>(insets.left * scale),
		static_cast<float>(insets.right * scale)};
}
