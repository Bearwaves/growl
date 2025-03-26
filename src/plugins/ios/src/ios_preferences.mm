#include "ios_system.h"
#include <Foundation/Foundation.h>

using Growl::IOSPreferences;
using Growl::Preferences;

IOSPreferences::IOSPreferences(bool ubiquitous, bool shared, nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, ubiquitous(ubiquitous) {}

IOSPreferences::~IOSPreferences() {
	store();
}

void IOSPreferences::store() {
	auto json_string = data().dump();
	auto key = isShared() ? @"preferences_shared" : @"preferences_local";
	NSString* json_ns_string =
		[NSString stringWithUTF8String:json_string.c_str()];
	if (ubiquitous) {
		[[NSUbiquitousKeyValueStore defaultStore] setString:json_ns_string
													 forKey:key];
	} else {
		[[NSUserDefaults standardUserDefaults] setValue:json_ns_string
												 forKey:key];
	}
}
