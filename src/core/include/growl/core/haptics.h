#pragma once

#include "growl/core/error.h"

namespace Growl {

enum class HapticsEventType {
	Rumble,
	TriggerRumble,
	Pattern,
};

struct HapticsEvent {
	HapticsEventType type = HapticsEventType::Rumble;
	double duration = 0.f;
	float intensity[2] = {0.f, 0.f};
};

class HapticsDevice {
public:
	virtual ~HapticsDevice() = default;

	virtual bool supportsEventType(HapticsEventType type) = 0;

	virtual Error triggerEvent(HapticsEvent event) = 0;
};

} // namespace Growl
