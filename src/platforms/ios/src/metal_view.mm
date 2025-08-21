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
		withEventType:(Growl::PointerEventType)type {
	for (UITouch* touch in touches) {
		CGPoint point = [touch locationInView:self];
		point.x *= self.contentScaleFactor;
		point.y *= self.contentScaleFactor;
		auto event = Growl::InputEvent(
			Growl::InputEventType::Touch,
			Growl::InputTouchEvent{
				type, static_cast<int>(point.x), static_cast<int>(point.y)});
		system->onEvent(event);
	}
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::PointerEventType::Down];
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::PointerEventType::Move];
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	[self handleTouches:touches withEventType:Growl::PointerEventType::Up];
}

@end
