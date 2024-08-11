#include "growl/core/config.h"
#include "growl/core/game/game.h"
#ifdef GROWL_DESKTOP
#include "growl/platforms/desktop/game_adapter.h"
#elif GROWL_IOS
#include "growl/platforms/ios/app_delegate.h"
#include <UIKit/UIKit.h>
#include <exception>
#elif GROWL_WEB
#include "growl/platforms/web/game_adapter.h"
#endif
#include "test_app_game.h"
#include <memory>

std::unique_ptr<Growl::Game> createGame() {
	return std::make_unique<Growl::TestAppGame>();
}

int main(int argc, char** argv) {
#ifdef GROWL_DESKTOP
	auto adapter = Growl::GameAdapter{
		std::make_unique<Growl::TestAppGame>(),
		Growl::Config{"Growl Test App", 1000, 1000, true}};
	adapter.run();
#elif GROWL_IOS
	NSString* appDelegateClassName;
	@autoreleasepool {
		try {
			appDelegateClassName = NSStringFromClass([GrowlAppDelegate class]);
			return UIApplicationMain(argc, argv, nil, appDelegateClassName);
		} catch (std::exception& e) {
			NSLog(@"Got exception: %s", e.what());
		}
	}
#elif GROWL_WEB
	auto adapter = Growl::GameAdapter{
		std::make_unique<Growl::TestAppGame>(),
		Growl::WindowConfig{"Growl Test App", 1000, 1000, true}};
	adapter.run();
#endif
}
