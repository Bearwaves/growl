#import "growl/platforms/ios/view_controller.h"
#import "growl/core/api/api.h"
#import "growl/core/frame_timer.h"
#import "growl/core/game/game.h"
#import "growl/core/graphics/window.h"
#import "growl/platforms/ios/metal_view.h"
#import "growl/scene/scene.h"
#import <iostream>
#import <memory>

// Forward declarations for the linker.
void initIOSPlugin(Growl::API& api);
void initSoLoudPlugin(Growl::API& api);
void initMetalPlugin(Growl::API& api);
void initLuaPlugin(Growl::API& api);
std::unique_ptr<Growl::Game> createGame();

@implementation GrowlViewController {
	CADisplayLink* displayLink;
	std::unique_ptr<Growl::API> api;
	std::unique_ptr<Growl::Game> game;

	id _pausedObserver;
	id _resumedObserver;
}

- (void)viewDidLoad {
	[super viewDidLoad];

	api = std::make_unique<Growl::API>();
	game = createGame();

	initIOSPlugin(*api);
	initSoLoudPlugin(*api);
	initMetalPlugin(*api);
	initLuaPlugin(*api);
	game->setAPI(api.get());
	api->setFrameTimer(std::make_unique<Growl::FrameTimer>());

	auto& systemInternal =
		static_cast<Growl::SystemAPIInternal&>(api->system());
	auto& graphicsInternal =
		static_cast<Growl::GraphicsAPIInternal&>(api->graphics());
	auto& audioInternal = static_cast<Growl::AudioAPIInternal&>(api->audio());
	auto& scriptingInternal =
		static_cast<Growl::ScriptingAPIInternal&>(api->scripting());

	if (auto err = systemInternal.init(game->getConfig())) {
		std::cout << "Failed to init system API: " << err.get()->message()
				  << std::endl;
		exit(1);
	}

	systemInternal.setDarkMode(
		self.traitCollection.userInterfaceStyle == UIUserInterfaceStyleDark);

	_resumedObserver = [[[NSNotificationCenter defaultCenter]
		addObserverForName:UIApplicationDidBecomeActiveNotification
					object:nil
					 queue:[NSOperationQueue currentQueue]
				usingBlock:^(NSNotification* _Nonnull notification) {
				  systemInternal.log("ViewController", "Resume");
				  systemInternal.resume();
				}] retain];

	_pausedObserver = [[[NSNotificationCenter defaultCenter]
		addObserverForName:UIApplicationDidEnterBackgroundNotification
					object:nil
					 queue:[NSOperationQueue currentQueue]
				usingBlock:^(NSNotification* _Nonnull notification) {
				  systemInternal.log("ViewController", "Pause");
				  systemInternal.pause();
				}] retain];

	if (auto err = graphicsInternal.init(game->getConfig())) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init graphics API: {}", err.get()->message());
		exit(2);
	}
	if (auto err = audioInternal.init(game->getConfig())) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init audio API: {}", err.get()->message());
		exit(3);
	}
	if (auto err = scriptingInternal.init(game->getConfig())) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init scripting API: {}", err.get()->message());
		exit(4);
	}

	if (auto err = Growl::initSceneGraph(*api)) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to init scene graph: {}", err.get()->message());
		exit(5);
	}

	api->system().log("ViewController", "iOS view controller created");

	if (auto err = graphicsInternal.setWindow(game->getConfig()); err) {
		api->system().log(
			Growl::LogLevel::Fatal, "GameAdapter",
			"Failed to create window: {}", err.get()->message());
		return;
	}

	[(GrowlMetalView*)self.view setSystemAPI:&systemInternal];

	if (auto err = game->init()) {
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
		  game->resize(w, h);
		}
						completion:^(
							id<UIViewControllerTransitionCoordinatorContext> _Nonnull context){}];
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)render {
	double delta_time = api->frameTimer().frame();
	api->system().tick();
	if (api->system().isPaused()) {
		return;
	}
	game->tick(delta_time);
	static_cast<Growl::GraphicsAPIInternal&>(api->graphics()).begin();
	game->render(delta_time);
	static_cast<Growl::GraphicsAPIInternal&>(api->graphics()).end();
}

- (void)viewDidAppear:(BOOL)animated {
	[displayLink setPaused:NO];
}

- (void)viewWillDisappear:(BOOL)animated {
	[displayLink setPaused:YES];
}

- (void)viewWillAppear:(BOOL)animated {
	int w = [[UIScreen mainScreen] bounds].size.width *
			self.view.contentScaleFactor;
	int h = [[UIScreen mainScreen] bounds].size.height *
			self.view.contentScaleFactor;
	game->resize(w, h);
}

- (void)willTransitionToTraitCollection:(UITraitCollection*)newCollection
			  withTransitionCoordinator:
				  (id<UIViewControllerTransitionCoordinator>)coordinator {
	auto& systemInternal =
		static_cast<Growl::SystemAPIInternal&>(api->system());
	systemInternal.setDarkMode(
		newCollection.userInterfaceStyle == UIUserInterfaceStyleDark);
}

- (void)dispose {
	if (_resumedObserver) {
		[[NSNotificationCenter defaultCenter] removeObserver:_resumedObserver];
		[_resumedObserver release];
	}
	if (_pausedObserver) {
		[[NSNotificationCenter defaultCenter] removeObserver:_pausedObserver];
		[_pausedObserver release];
	}
	if (auto err = game->dispose()) {
		api->system().log(
			Growl::LogLevel::Fatal, "ViewController",
			"Failed to dispose game: {}", err.get()->message());
		exit(4);
	}
	game.reset();
	api->system().log("ViewController", "Shutting down ViewController");
	static_cast<Growl::ScriptingAPIInternal&>(api->scripting()).dispose();
	static_cast<Growl::AudioAPIInternal&>(api->audio()).dispose();
	static_cast<Growl::GraphicsAPIInternal&>(api->graphics()).dispose();
	static_cast<Growl::SystemAPIInternal&>(api->system()).dispose();
}

@end
