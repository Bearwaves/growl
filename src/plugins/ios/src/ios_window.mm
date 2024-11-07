#include "ios_window.h"

using Growl::IOSWindow;
using Growl::WindowSafeAreaInsets;

void* IOSWindow::getMetalLayer() {
	return native.rootViewController.view.layer;
}

WindowSafeAreaInsets IOSWindow::getSafeAreaInsets() {
	auto insets = native.safeAreaInsets;
	return WindowSafeAreaInsets{
		static_cast<float>(insets.top), static_cast<float>(insets.bottom),
		static_cast<float>(insets.left), static_cast<float>(insets.right)};
}
