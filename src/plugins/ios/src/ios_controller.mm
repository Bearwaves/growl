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
