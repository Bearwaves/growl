#import "growl/platforms/ios/scene_delegate.h"
#import "growl/platforms/ios/view_controller.h"

@implementation GrowlSceneDelegate

- (void)scene:(UIScene*)scene
	willConnectToSession:(UISceneSession*)session
				 options:(UISceneConnectionOptions*)connectionOptions {
	self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene*)scene];
	self.window.rootViewController = [[GrowlViewController alloc] init];
	self.window.backgroundColor = [UIColor whiteColor];
	[self.window makeKeyAndVisible];
}

- (void)sceneDidDisconnect:(UIScene*)scene {
	[static_cast<GrowlViewController*>(self.window.rootViewController) dispose];
}

@end
