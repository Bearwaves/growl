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
	Swing,
	SwingIn,
	SwingOut,
};

float interpolate(
	float from, float to, float factor,
	Interpolation interpolation = Interpolation::Linear, float scale = 1.f);

Color interpolate(
	Color from, Color to, float factor,
	Interpolation interpolation = Interpolation::Linear, float scale = 1.f);

} // namespace Growl
