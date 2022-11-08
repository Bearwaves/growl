#import "growl/platforms/ios/metal_view.h"

@implementation GrowlMetalView {
	Growl::SystemAPIInternal* system;
}

+ (Class)layerClass {
	return [CAMetalLayer class];
}

- (void)setSystemAPI:(Growl::SystemAPIInternal*)system {
	self->system = system;
}

- (void)handleTouches:(NSSet<UITouch*>*)touches
		withEventType:(Growl::TouchEventType)type {
	for (UITouch* touch in touches) {
		CGPoint point = [touch locationInView:self];
		point.x *= self.contentScaleFactor;
		point.y *= self.contentScaleFactor;
		system->onTouch(Growl::InputTouchEvent{
			.type = type,
			.touchX = static_cast<int>(point.x),
			.touchY = static_cast<int>(point.y)});
	}
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::TouchEventType::Down];
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::TouchEventType::Move];
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::TouchEventType::Up];
}

@end
