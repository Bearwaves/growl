#import "growl/platforms/ios/view_controller.h"
#import "growl/core/game/game.h"
#import "growl/core/graphics/window.h"
#import "growl/platforms/ios/metal_view.h"
#import <memory>

// Forward declarations for the linker.
void initIOSPlugin(Growl::API& api);
void initSoLoudPlugin(Growl::API& api);
void initMetalPlugin(Growl::API& api);
std::unique_ptr<Growl::Game> createGame();

@implementation GrowlViewController {
	CADisplayLink* displayLink;
	std::unique_ptr<Growl::API> api;
	std::unique_ptr<Growl::Game> game;
}

- (void)viewDidLoad {
	[super viewDidLoad];

	api = std::make_unique<Growl::API>();
	game = createGame();

	initIOSPlugin(*api);
	initSoLoudPlugin(*api);
	initMetalPlugin(*api);
	game->setAPI(api.get());

	auto& systemInternal =
		static_cast<Growl::SystemAPIInternal&>(api->system());
	auto& graphicsInternal =
		static_cast<Growl::GraphicsAPIInternal&>(api->graphics());
	auto& audioInternal = static_cast<Growl::AudioAPIInternal&>(api->audio());

	if (auto err = systemInternal.init(); err) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}
	if (auto err = graphicsInternal.init(); err) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init graphics API: {}", err.get()->message());
		exit(2);
	}
	if (auto err = audioInternal.init(); err) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init audio API: {}", err.get()->message());
		exit(3);
	}
	api->system().log("ViewController", "iOS view controller created");

	if (auto err =
			graphicsInternal.setWindow(Growl::WindowConfig{"", 0, 0, false});
		err) {
		api->system().log(
			Growl::LogLevel::Fatal, "GameAdapter",
			"Failed to create window: {}", err.get()->message());
		return;
	}

	[(GrowlMetalView*)self.view setSystemAPI:&systemInternal];

	if (auto err = game->init(); err) {
		api->system().log(
			Growl::LogLevel::Fatal, "GameAdapter", "Failed to init game: {}",
			err.get()->message());
		return;
	}

	displayLink =
		[[CADisplayLink displayLinkWithTarget:self
									 selector:@selector(render)] retain];
	[displayLink setPaused:YES];
	[displayLink addToRunLoop:[NSRunLoop currentRunLoop]
					  forMode:NSDefaultRunLoopMode];
}

- (void)loadView {
	[super loadView];
	self.view = [[GrowlMetalView alloc] initWithFrame:self.view.bounds];
	[self.view setAutoresizingMask:UIViewAutoresizingFlexibleHeight |
								   UIViewAutoresizingFlexibleWidth];
	UIScreen* screen = self.view.window.screen ? self.view.window.screen
											   : [UIScreen mainScreen];
	[self.view setContentScaleFactor:screen.nativeScale];
}

- (void)viewWillTransitionToSize:(CGSize)size
	   withTransitionCoordinator:
		   (id<UIViewControllerTransitionCoordinator>)coordinator {
	[super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
	int w = size.width * self.view.contentScaleFactor;
	int h = size.height * self.view.contentScaleFactor;
	api->system().log(
		Growl::LogLevel::Debug, "ViewController",
		"Got new screen size ({}, {}).", w, h);
	[coordinator
		animateAlongsideTransition:^(
			id<UIViewControllerTransitionCoordinatorContext> _Nonnull context) {
		  static_cast<Growl::GraphicsAPIInternal&>(api->graphics())
			  .onWindowResize(w, h);
		}
						completion:^(
							id<UIViewControllerTransitionCoordinatorContext> _Nonnull context){}];
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)render {
	api->system().tick();
	static_cast<Growl::GraphicsAPIInternal&>(api->graphics()).begin();
	game->render();
	static_cast<Growl::GraphicsAPIInternal&>(api->graphics()).end();
}

- (void)viewDidAppear:(BOOL)animated {
	[displayLink setPaused:NO];
}

- (void)viewWillDisappear:(BOOL)animated {
	[displayLink setPaused:YES];
}

@end
