#pragma once

#import "growl/core/assets/image.h"
#import <UIKit/UIKit.h>

@interface GrowlShareableImage : NSObject <UIActivityItemSource>

- (id)initWithImage:(Growl::Image&)image title:(NSString*)title;

@end
