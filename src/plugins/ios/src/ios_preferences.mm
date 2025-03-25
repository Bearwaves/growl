#include "ios_system.h"
#include <Foundation/Foundation.h>

using Growl::IOSPreferences;
using Growl::Preferences;

IOSPreferences::IOSPreferences(
	IOSSystemAPI& api, bool shared, nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, api{api} {}

IOSPreferences::~IOSPreferences() {
	store();
}

void IOSPreferences::store() {
	auto json_string = data().dump();
	NSString* json_ns_string =
		[NSString stringWithUTF8String:json_string.c_str()];
	if (isShared()) {
		[[NSUbiquitousKeyValueStore defaultStore] setString:json_ns_string
													 forKey:@"preferences"];
	} else {
		[[NSUserDefaults standardUserDefaults] setValue:json_ns_string
												 forKey:@"preferences"];
	}
}
