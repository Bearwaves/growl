#import "ios_shareable_image.h"
#include "fpng.h"
#import <LinkPresentation/LPLinkMetadata.h>

@implementation GrowlShareableImage {
	std::vector<unsigned char> image_png;
	UIImage* ui_image;
	NSString* title;
	NSURL* file_url;
}

- (instancetype)initWithImage:(Growl::Image&)image title:(NSString*)title {
	self = [super init];
	NSURL* tmp = [NSURL fileURLWithPath:NSTemporaryDirectory() isDirectory:YES];
	file_url = [tmp
		URLByAppendingPathComponent:[title stringByAppendingString:@".png"]];

	fpng::fpng_encode_image_to_memory(
		image.getRaw(), image.getWidth(), image.getHeight(),
		image.getChannels(), image_png);

	fpng::fpng_encode_image_to_file(
		file_url.path.UTF8String, image.getRaw(), image.getWidth(),
		image.getHeight(), image.getChannels());

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
	return file_url;
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
		[[NSItemProvider alloc] initWithContentsOfURL:file_url];
	auto metadata = [[LPLinkMetadata alloc] init];
	metadata.title = title;
	metadata.iconProvider = imageProvider;
	metadata.imageProvider = imageProvider;
	metadata.originalURL = file_url;
	metadata.URL = file_url;

	return metadata;
}

- (void)deleteTempFile {
	if (file_url &&
		[[NSFileManager defaultManager] fileExistsAtPath:file_url.path]) {
		[[NSFileManager defaultManager] removeItemAtURL:file_url error:nil];
	}
}

@end
