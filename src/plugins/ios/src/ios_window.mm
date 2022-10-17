#include "ios_window.h"

using Growl::IOSWindow;

void* IOSWindow::getMetalLayer() {
	return native.rootViewController.view.layer;
}
