#pragma once

#include "growl/core/error.h"
#include <vector>

namespace Growl {

enum class HapticsEventType {
	Rumble,
	TriggerRumble,
	Pattern,
};

struct HapticsEvent {
	HapticsEventType type = HapticsEventType::Rumble;
	double duration = 0.f;
	// For rumble this corresponds to left and right; in a pattern
	// it is intensity and sharpness.
	float intensity[2] = {0.f, 0.f};
	// Only used in patterns. Offset from end of previous event;
	// overlapping events are not supported.
	float offset = 0.f;
};

class HapticsDevice {
public:
	virtual ~HapticsDevice() = default;

	virtual bool supportsEventType(HapticsEventType type) = 0;

	virtual Error playEvent(HapticsEvent event, float intensity = 1.f) = 0;

	virtual Error
	playPattern(std::vector<HapticsEvent> pattern, float intensity = 1.f) = 0;
};

} // namespace Growl
