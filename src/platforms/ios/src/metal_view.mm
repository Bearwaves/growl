#import "growl/platforms/ios/metal_view.h"

@implementation GrowlMetalView

+ (Class)layerClass {
	return [CAMetalLayer class];
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
}

@end
