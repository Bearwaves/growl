#include "android_system.h"

using Growl::AndroidController;
using Growl::AndroidHaptics;
using Growl::ControllerLayout;
using Growl::HapticsDevice;

AndroidController::AndroidController(
	android_app* app, int id, std::unique_ptr<Paddleboat_Controller_Info> info)
	: id{id}
	, info{std::move(info)} {
	this->haptics = std::make_unique<AndroidHaptics>(app, this->info.get());
	switch (this->info->controllerFlags & PADDLEBOAT_CONTROLLER_LAYOUT_MASK) {
	case PADDLEBOAT_CONTROLLER_LAYOUT_SHAPES:
		this->layout = ControllerLayout::PS;
		break;
	case PADDLEBOAT_CONTROLLER_LAYOUT_REVERSE:
		this->layout = ControllerLayout::ABXY;
		break;
	default:
		this->layout = ControllerLayout::BAYX;
		break;
	}
}

HapticsDevice* AndroidController::getHaptics() {
	return haptics.get();
}