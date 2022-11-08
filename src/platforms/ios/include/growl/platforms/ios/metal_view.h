#import "growl/core/api/api_internal.h"
#import <UIKit/UIKit.h>

@interface GrowlMetalView : UIView

- (void)setSystemAPI:(Growl::SystemAPIInternal*)system;
- (void)handleTouches:(NSSet<UITouch*>*)touches
		withEventType:(Growl::PointerEventType)type;

@end
