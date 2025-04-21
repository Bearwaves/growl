#include "ios_haptics.h"
#include "growl/core/api/api.h"
#include "growl/core/error.h"
#include "growl/core/haptics.h"
#include "ios_error.h"
#include <CoreHaptics/CoreHaptics.h>
#include <GameController/GameController.h>

using Growl::Error;
using Growl::HapticsEvent;
using Growl::IOSError;
using Growl::IOSHapticsDevice;
using Growl::Result;

IOSHapticsDevice::IOSHapticsDevice(SystemAPI& system, GCController* controller)
	: system{system} {
	if (controller) {
		setGameController(controller);
	} else if (CHHapticEngine.capabilitiesForHardware.supportsHaptics) {
		// System haptics
		NSError* error;
		this->device =
			[[[CHHapticEngine alloc] initAndReturnError:&error] retain];
		[this->device setResetHandler:^{
		  NSError* error;
		  [this->device startAndReturnError:&error];
		  if (error) {
			  system.log(
				  LogLevel::Error, "IOSHapticsDevice",
				  "Failed to start device engine: {}",
				  std::string([error.description UTF8String]));
		  }
		}];
		[this->device resetHandler]();
	}
}

IOSHapticsDevice::~IOSHapticsDevice() {
	this->controller = nullptr;
	stopAllEngines();
}

bool IOSHapticsDevice::supportsEventType(HapticsEventType type) {
	switch (type) {
	case HapticsEventType::Rumble:
		return left_handle;
	case HapticsEventType::TriggerRumble:
		return left_trigger;
	case HapticsEventType::Pattern:
		return device;
	default:
		break;
	}
	return false;
}

Error IOSHapticsDevice::playEvent(HapticsEvent event) {
	if (!supportsEventType(event.type)) {
		return nullptr;
	}
	switch (event.type) {
	case HapticsEventType::Rumble:
		if (auto err =
				doRumbleEvent(event, this->left_handle, this->right_handle)) {
			return Error(
				std::make_unique<IOSError>(
					"Failed to play rumble event: " + err->message()));
		}
		return nullptr;
	case HapticsEventType::TriggerRumble:
		if (auto err =
				doRumbleEvent(event, this->left_trigger, this->right_trigger)) {
			return Error(
				std::make_unique<IOSError>(
					"Failed to play trigger rumble event: " + err->message()));
		}
		return nullptr;
	default:
		break;
	}
	return nullptr;
}

Error IOSHapticsDevice::playPattern(std::vector<HapticsEvent> pattern) {
	if (!supportsEventType(HapticsEventType::Pattern)) {
		return nullptr;
	}

	NSMutableArray* events = [NSMutableArray array];
	for (auto& event : pattern) {
		[events addObject:@{
			CHHapticPatternKeyEvent : @{
				CHHapticPatternKeyEventType : event.duration
					? CHHapticEventTypeHapticContinuous
					: CHHapticEventTypeHapticTransient,
				CHHapticPatternKeyTime : @(event.offset),
				CHHapticPatternKeyEventDuration : @(event.duration),
				CHHapticPatternKeyEventParameters : @[
					@{
						CHHapticPatternKeyParameterID :
							CHHapticEventParameterIDHapticIntensity,
						CHHapticPatternKeyParameterValue : @(event.intensity[0])
					},
					@{
						CHHapticPatternKeyParameterID :
							CHHapticEventParameterIDHapticSharpness,
						CHHapticPatternKeyParameterValue : @(event.intensity[1])
					}
				]
			}
		}];
	}

	NSDictionary* haptic_dict = @{CHHapticPatternKeyPattern : events};

	NSError* error = nil;
	CHHapticPattern* haptic_pattern =
		[[CHHapticPattern alloc] initWithDictionary:haptic_dict error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create pattern: " +
				std::string([error.description UTF8String])));
	}
	id<CHHapticPatternPlayer> player =
		[device createPlayerWithPattern:haptic_pattern error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create pattern player: " +
				std::string([error.description UTF8String])));
	}

	[player startAtTime:0 error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to play pattern: " +
				std::string([error.description UTF8String])));
	}

	return nullptr;
}

void IOSHapticsDevice::setGameController(GCController* controller) {
	this->controller = controller;
	stopAllEngines();

	if (this->controller && this->controller.haptics) {
		auto localities = this->controller.haptics.supportedLocalities;
		if ([localities containsObject:GCHapticsLocalityHandles]) {
			this->left_handle = [[this->controller.haptics
				createEngineWithLocality:GCHapticsLocalityLeftHandle] retain];
			[this->left_handle setResetHandler:^{
			  NSError* error;
			  [this->left_handle startAndReturnError:&error];
			  if (error) {
				  system.log(
					  LogLevel::Error, "IOSHapticsDevice",
					  "Failed to start left handle engine: {}",
					  std::string([error.description UTF8String]));
			  }
			}];
			[this->left_handle resetHandler]();

			this->right_handle = [[this->controller.haptics
				createEngineWithLocality:GCHapticsLocalityRightHandle] retain];
			[this->right_handle setResetHandler:^{
			  NSError* error;
			  [this->right_handle startAndReturnError:&error];
			  if (error) {
				  system.log(
					  LogLevel::Error, "IOSHapticsDevice",
					  "Failed to start right handle engine: {}",
					  std::string([error.description UTF8String]));
			  }
			}];
			[this->right_handle resetHandler]();
			system.log("IOSHapticsDevice", "Controller rumble supported");
		}
		if ([localities containsObject:GCHapticsLocalityTriggers]) {
			this->left_trigger = [[this->controller.haptics
				createEngineWithLocality:GCHapticsLocalityLeftTrigger] retain];
			[this->left_trigger setResetHandler:^{
			  NSError* error;
			  [this->left_trigger startAndReturnError:&error];
			  if (error) {
				  system.log(
					  LogLevel::Error, "IOSHapticsDevice",
					  "Failed to start left trigger engine: {}",
					  std::string([error.description UTF8String]));
			  }
			}];
			[this->left_trigger resetHandler]();

			this->right_trigger = [[this->controller.haptics
				createEngineWithLocality:GCHapticsLocalityRightTrigger] retain];
			[this->right_trigger setResetHandler:^{
			  NSError* error;
			  [this->right_trigger startAndReturnError:&error];
			  if (error) {
				  system.log(
					  LogLevel::Error, "IOSHapticsDevice",
					  "Failed to start right trigger engine: {}",
					  std::string([error.description UTF8String]));
			  }
			}];
			[this->right_trigger resetHandler]();
			system.log(
				"IOSHapticsDevice", "Controller trigger rumble supported");
		}
	}
}

void IOSHapticsDevice::stopAllEngines() {
	dispatch_group_t group = dispatch_group_create();

	if (this->device) {
		dispatch_group_enter(group);
		[this->device stopWithCompletionHandler:^(NSError* _Nullable error) {
		  if (error) {
			  system.log(
				  LogLevel::Warn, "IOSHapticsDevice",
				  "Failed to stop device haptics engine: {}",
				  std::string([error.description UTF8String]));
		  }
		  [this->device release];
		  this->device = nil;
		  dispatch_group_leave(group);
		}];
	}

	if (this->left_handle) {
		dispatch_group_enter(group);
		[this->left_handle
			stopWithCompletionHandler:^(NSError* _Nullable error) {
			  if (error) {
				  system.log(
					  LogLevel::Warn, "IOSHapticsDevice",
					  "Failed to stop left handle haptics engine: {}",
					  std::string([error.description UTF8String]));
			  }
			  [this->left_handle release];
			  this->left_handle = nil;
			  dispatch_group_leave(group);
			}];
	}

	if (this->right_handle) {
		dispatch_group_enter(group);
		[this->right_handle
			stopWithCompletionHandler:^(NSError* _Nullable error) {
			  if (error) {
				  system.log(
					  LogLevel::Warn, "IOSHapticsDevice",
					  "Failed to stop right handle haptics engine: {}",
					  std::string([error.description UTF8String]));
			  }
			  [this->right_handle release];
			  this->right_handle = nullptr;
			  dispatch_group_leave(group);
			}];
	}

	if (this->left_trigger) {
		dispatch_group_enter(group);
		[this->left_trigger
			stopWithCompletionHandler:^(NSError* _Nullable error) {
			  if (error) {
				  system.log(
					  LogLevel::Warn, "IOSHapticsDevice",
					  "Failed to stop left trigger haptics engine: {}",
					  std::string([error.description UTF8String]));
			  }
			  [this->left_trigger release];
			  this->left_trigger = nullptr;
			  dispatch_group_leave(group);
			}];
	}

	if (this->right_trigger) {
		dispatch_group_enter(group);
		[this->right_trigger
			stopWithCompletionHandler:^(NSError* _Nullable error) {
			  if (error) {
				  system.log(
					  LogLevel::Warn, "IOSHapticsDevice",
					  "Failed to stop right trigger haptics engine: {}",
					  std::string([error.description UTF8String]));
			  }
			  [this->right_trigger release];
			  this->right_trigger = nullptr;
			  dispatch_group_leave(group);
			}];
	}

	dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

Error IOSHapticsDevice::doRumbleEvent(
	HapticsEvent& event, CHHapticEngine* left, CHHapticEngine* right) {
	auto left_pattern_res = createRumblePattern(event, 0);
	if (!left_pattern_res) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create left rumble pattern: " +
				left_pattern_res.error()->message()));
	}
	auto right_pattern_res = createRumblePattern(event, 1);
	if (!right_pattern_res) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create right rumble pattern: " +
				right_pattern_res.error()->message()));
	}
	NSError* error = nil;
	auto left_player = [left createPlayerWithPattern:left_pattern_res.get()
											   error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create left rumble player: " +
				std::string([error.description UTF8String])));
	}
	auto right_player = [right createPlayerWithPattern:right_pattern_res.get()
												 error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create right rumble player: " +
				std::string([error.description UTF8String])));
	}
	[left_player startAtTime:0 error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to play left rumble: " +
				std::string([error.description UTF8String])));
	}
	[right_player startAtTime:0 error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to play right rumble: " +
				std::string([error.description UTF8String])));
	}
	return nullptr;
}

Result<CHHapticPattern*>
IOSHapticsDevice::createRumblePattern(HapticsEvent& event, int loc) {
	NSError* error = nil;
	auto pattern = [[CHHapticPattern alloc] initWithEvents:@[
		[[CHHapticEvent alloc]
			initWithEventType:CHHapticEventTypeHapticContinuous
				   parameters:@[ [[CHHapticEventParameter alloc]
								  initWithParameterID:
									  CHHapticEventParameterIDHapticIntensity
												value:event.intensity[loc]] ]
				 relativeTime:0
					 duration:event.duration]
	]
												parameters:@[]
													 error:&error];
	if (error) {
		return Error(
			std::make_unique<IOSError>(
				"Failed to create rumble pattern: " +
				std::string([error.description UTF8String])));
	}
	return pattern;
}
