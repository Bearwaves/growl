#include "android_system.h"

using Growl::AndroidController;
using Growl::AndroidHaptics;
using Growl::HapticsDevice;

AndroidController::AndroidController(
	android_app* app, int id, std::unique_ptr<Paddleboat_Controller_Info> info)
	: id{id}
	, info{std::move(info)} {
	this->haptics = std::make_unique<AndroidHaptics>(app, this->info.get());
}

HapticsDevice* AndroidController::getHaptics() {
	return haptics.get();
}