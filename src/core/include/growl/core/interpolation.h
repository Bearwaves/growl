#pragma once

namespace Growl {

struct Color;

enum class Interpolation {
	Linear,
	Smooth,
	Smoother,
	Sine,
	SineIn,
	SineOut,
	CircleIn,
	CircleOut,
};

float interpolate(
	float from, float to, float factor,
	Interpolation interpolation = Interpolation::Linear);

Color interpolate(
	Color from, Color to, float factor,
	Interpolation interpolation = Interpolation::Linear);

} // namespace Growl
