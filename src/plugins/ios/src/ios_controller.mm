#include "ios_haptics.h"
#include "ios_system.h"

using Growl::HapticsDevice;
using Growl::IOSController;
using Growl::IOSHapticsDevice;
using Growl::SystemAPI;

IOSController::IOSController(
	SystemAPI& system, GCController* controller, int id)
	: controller{controller}
	, id{id} {
	this->haptics = std::make_unique<IOSHapticsDevice>(system, controller);
	if ([controller.productCategory containsString:@"Switch"]) {
		this->layout = ControllerLayout::ABXY;
	} else if ([controller.productCategory containsString:@"DualSense"] ||
			   [controller.productCategory containsString:@"DualShock"]) {
		this->layout = ControllerLayout::PS;
	} else {
		this->layout = ControllerLayout::BAYX;
	}
}

IOSController::~IOSController() {
	this->haptics.reset();
}

HapticsDevice* IOSController::getHaptics() {
	return this->haptics.get();
}

void IOSController::resumeHaptics() {
	this->haptics->restart();
}
