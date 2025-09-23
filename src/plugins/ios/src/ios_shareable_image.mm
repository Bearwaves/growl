#import "ios_shareable_image.h"
#include "fpng.h"
#import <LinkPresentation/LPLinkMetadata.h>

@implementation GrowlShareableImage {
	std::vector<unsigned char> image_png;
	UIImage* ui_image;
	NSString* title;
}

- (id)initWithImage:(Growl::Image&)image title:(NSString*)title {
	fpng::fpng_encode_image_to_memory(
		image.getRaw(), image.getWidth(), image.getHeight(),
		image.getChannels(), image_png);
	ui_image =
		[UIImage imageWithData:[NSData dataWithBytesNoCopy:image_png.data()
													length:image.getWidth() *
														   image.getHeight() *
														   image.getChannels()
											  freeWhenDone:false]];
	self->title = title;
	return self;
}

- (nullable id)activityViewController:
				   (nonnull UIActivityViewController*)activityViewController
				  itemForActivityType:(nullable UIActivityType)activityType {
	return ui_image;
}

- (nonnull id)activityViewControllerPlaceholderItem:
	(nonnull UIActivityViewController*)activityViewController {
	return ui_image;
}

- (UIImage*)activityViewController:
				(UIActivityViewController*)activityViewController
	 thumbnailImageForActivityType:(UIActivityType)activityType
					 suggestedSize:(CGSize)size {
	return ui_image;
}

- (NSString*)activityViewController:
				 (UIActivityViewController*)activityViewController
			 subjectForActivityType:(UIActivityType)activityType {
	return title;
}

- (LPLinkMetadata*)activityViewControllerLinkMetadata:
	(UIActivityViewController*)activityViewController {
	NSItemProvider* imageProvider =
		[[NSItemProvider alloc] initWithObject:ui_image];
	auto metadata = [[LPLinkMetadata alloc] init];
	metadata.title = title;
	metadata.iconProvider = imageProvider;
	metadata.imageProvider = imageProvider;
	metadata.originalURL = [NSURL URLWithString:@"PNG"];

	return metadata;
}

@end
