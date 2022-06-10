#include "assets_config.h"

void Growl::from_json(const json& j, AssetConfig& s) {
	if (j.contains("atlas")) {
		s.atlas = j.at("atlas").get<AtlasConfig>();
	}
	if (j.contains("font")) {
		s.font = j.at("font").get<FontConfig>();
	}
}

void Growl::from_json(const json& j, AtlasConfig& s) {
	s.padding = j.value("padding", s.padding);
}

void Growl::from_json(const json& j, FontConfig& s) {
	s.msdf = j.value("msdf", s.msdf);
	s.msdfSize = j.value("msdfSize", s.msdfSize);
}