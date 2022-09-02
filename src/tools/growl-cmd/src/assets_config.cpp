#include "assets_config.h"

void Growl::from_json(const json& j, AssetConfig& s) {
	if (j.contains("atlas")) {
		s.atlas = j.at("atlas").get<AtlasConfig>();
	}
	if (j.contains("font")) {
		s.font = j.at("font").get<FontConfig>();
	}
	s.text = j.value("text", s.text);
}

void Growl::from_json(const json& j, AtlasConfig& s) {
	s.padding = j.value("padding", s.padding);
	s.bleed_passes = j.value("bleedPasses", s.bleed_passes);
}

void Growl::from_json(const json& j, FontConfig& s) {
	s.msdf = j.value("msdf", s.msdf);
	s.msdfSize = j.value("msdfSize", s.msdfSize);
	s.msdfCharacters = j.value("msdfCharacters", s.msdfCharacters);
}
