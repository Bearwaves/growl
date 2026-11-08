#import "growl/platforms/ios/app_delegate.h"
#import "growl/platforms/ios/scene_delegate.h"
#import <Foundation/Foundation.h>

@implementation GrowlAppDelegate

- (BOOL)application:(UIApplication*)application
	didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
	return YES;
}

- (UISceneConfiguration*)application:(UIApplication*)application
	configurationForConnectingSceneSession:
		(UISceneSession*)connectingSceneSession
								   options:(UISceneConnectionOptions*)options {
	UISceneConfiguration* config = [[[UISceneConfiguration alloc]
		initWithName:@"Growl Config"
		 sessionRole:connectingSceneSession.role] autorelease];
	config.delegateClass = [GrowlSceneDelegate class];
	return config;
}

@end
