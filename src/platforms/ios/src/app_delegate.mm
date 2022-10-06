#import "growl/platforms/ios/app_delegate.h"
#import "growl/platforms/ios/view_controller.h"
#import <Foundation/Foundation.h>

@implementation GrowlAppDelegate

- (BOOL)application:(UIApplication*)application
	didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
	self.window =
		[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.rootViewController = [[GrowlViewController alloc] init];
	self.window.backgroundColor = [UIColor blackColor];
	[self.window makeKeyAndVisible];

	return YES;
}

- (void)applicationWillResignActive:(UIApplication*)application {
}

- (void)applicationDidEnterBackground:(UIApplication*)application {
}

- (void)applicationWillEnterForeground:(UIApplication*)application {
}

- (void)applicationDidBecomeActive:(UIApplication*)application {
}

@end
