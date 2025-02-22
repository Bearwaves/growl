#include "growl/core/interpolation.h"
#include "growl/core/graphics/color.h"
#include <cmath>

using Growl::Color;

// From LibGDX
// https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/math/Interpolation.java

float Growl::interpolate(
	float from, float to, float factor, Interpolation interpolation,
	float scale) {
	float a = factor;
	switch (interpolation) {
	case Interpolation::Linear:
		// a == factor already
		break;
	case Interpolation::Smooth:
		a = a * a * (3 - 2 * a);
		break;
	case Interpolation::Smoother:
		a = a * a * a * (a * (a * 6 - 15) + 10);
		break;
	case Interpolation::Sine:
		a = (1 - std::cos(a * M_PI)) / 2;
		break;
	case Interpolation::SineIn:
		a = 1 - std::cos(a * M_PI_2);
		break;
	case Interpolation::SineOut:
		a = std::cos(a * M_PI_2);
		break;
	case Interpolation::CircleIn:
		a = 1 - std::sqrt(1 - a * a);
		break;
	case Interpolation::CircleOut:
		a--;
		a = std::sqrt(1 - a * a);
		break;
	case Interpolation::Swing:
		if (a <= 0.5f) {
			a *= 2;
			a = a * a * ((scale + 1) * a - scale) / 2;
			break;
		}
		a--;
		a *= 2;
		a = a * a * ((scale + 1) * a + scale) / 2 + 1;
		break;
	case Interpolation::SwingIn:
		a = a * a * ((scale + 1) * a - scale);
		break;
	case Interpolation::SwingOut:
		a--;
		a = a * a * ((scale + 1) * a + scale) + 1;
		break;
	default:
		break;
	}
	return from + (to - from) * a;
}

Color Growl::interpolate(
	Color from, Color to, float factor, Interpolation interpolation,
	float scale) {
	return Color{
		interpolate(from.r, to.r, factor, interpolation, scale),
		interpolate(from.g, to.g, factor, interpolation, scale),
		interpolate(from.b, to.b, factor, interpolation, scale),
		interpolate(from.a, to.a, factor, interpolation, scale),
	};
}
