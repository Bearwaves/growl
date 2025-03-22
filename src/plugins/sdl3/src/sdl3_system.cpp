#include "sdl3_system.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "growl/core/api/api.h"
#include "growl/core/assets/file.h"
#include "growl/core/input/event.h"
#include "growl/core/input/processor.h"
#include "growl/core/log.h"
#include "sdl3_file.h"
#ifdef GROWL_IMGUI
#include "growl/core/imgui.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#endif
#include "sdl3_error.h"
#include "sdl3_window.h"
#include <memory>

using Growl::Error;
using Growl::File;
using Growl::InputEvent;
using Growl::Result;
using Growl::SDL3SystemAPI;
using Growl::Window;

Error SDL3SystemAPI::init(const Config& config) {
	if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
		return std::make_unique<SDL3Error>(SDL_GetError());
	}
	SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_INFO);
	if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
		return std::make_unique<SDL3Error>(SDL_GetError());
	}

#ifdef GROWL_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	imgui_io = &ImGui::GetIO();
	imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

	this->log("SDL3SystemAPI", "Initialised SDL system");

	running = true;
	return nullptr;
}

Result<std::unique_ptr<Window>>
SDL3SystemAPI::createWindow(const Config& config) {
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(
		props, SDL_PROP_WINDOW_CREATE_TITLE_STRING,
		config.window_title.c_str());
	SDL_SetNumberProperty(
		props, SDL_PROP_WINDOW_CREATE_X_NUMBER,
		config.window_centered ? SDL_WINDOWPOS_CENTERED : 0);
	SDL_SetNumberProperty(
		props, SDL_PROP_WINDOW_CREATE_Y_NUMBER,
		config.window_centered ? SDL_WINDOWPOS_CENTERED : 0);
	SDL_SetNumberProperty(
		props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, config.window_width);
	SDL_SetNumberProperty(
		props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, config.window_height);
	SDL_SetBooleanProperty(
		props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	SDL_SetBooleanProperty(
		props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
	SDL_SetBooleanProperty(
		props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
	SDL_Window* win = SDL_CreateWindowWithProperties(props);
	if (!win) {
		return Error(std::make_unique<SDL3Error>(SDL_GetError()));
	}

	debug_mode_key = getScancode(config.debug_mode_key);
	setDarkMode(SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK);

	return std::unique_ptr<Window>(std::make_unique<SDL3Window>(win));
}

void SDL3SystemAPI::tick() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		bool scene_focused = true;
#ifdef GROWL_IMGUI
		if (api.imguiVisible()) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			scene_focused = imGuiGameWindowFocused();
		}
#endif
		switch (event.type) {
		case SDL_EVENT_QUIT: {
			log("SDL3SystemAPI", "Got stop signal");
			running = false;
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_WHEEL:
			if (scene_focused) {
				handleMouseEvent(event);
			}
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			if (scene_focused || event.key.scancode == debug_mode_key) {
				handleKeyboardEvent(event);
			}
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			handleControllerEvent(event);
			break;
		case SDL_EVENT_GAMEPAD_ADDED:
			openGameController(event.cdevice.which);
			break;
		case SDL_EVENT_SYSTEM_THEME_CHANGED:
			setDarkMode(SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK);
			break;
		case SDL_EVENT_WINDOW_RESIZED: {
#ifdef GROWL_IMGUI
			if (api.imguiVisible()) {
				break;
			}
#endif
			auto window = SDL_GetWindowFromID(event.window.windowID);
			if (window) {
				// SDL event doesn't account for HiDPI
				SDL_GetWindowSizeInPixels(
					window, &resize_width, &resize_height);
			} else {
				resize_width = event.window.data1;
				resize_height = event.window.data2;
			}
			break;
		}
		}
	}
#ifdef GROWL_IMGUI
	if (api.imguiVisible()) {
		if ((imgui_resize_window || imGuiGameWindowResized())) {
			imGuiGameWindowSize(&resize_width, &resize_height);
			imgui_resize_window = 0;
		}
	} else if (imgui_resize_window) {
		auto window = SDL_GetWindowFromID(imgui_resize_window);
		if (window) {
			SDL_GetWindowSizeInPixels(window, &resize_width, &resize_height);
		}
		imgui_resize_window = 0;
	}
#endif
}

void SDL3SystemAPI::stop() {
	SDL_Quit();
}

void SDL3SystemAPI::dispose() {
	controller = nullptr;
	SDL_Quit();
#ifdef GROWL_IMGUI
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
#endif
}

bool SDL3SystemAPI::didResize(int* width, int* height) {
	*width = resize_width;
	*height = resize_height;
	resize_width = 0;
	resize_height = 0;
	return *width || *height;
}

void SDL3SystemAPI::setLogLevel(LogLevel log_level) {
	SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, getLogPriority(log_level));
}

Result<std::unique_ptr<File>>
SDL3SystemAPI::openFile(std::string path, size_t start, size_t end) {
	auto fp = SDL_IOFromFile(path.c_str(), "rb");
	if (!fp) {
		return Error(std::make_unique<SDL3Error>(SDL_GetError()));
	}

	if (end == 0) {
		end = SDL_GetIOSize(fp);
	}

	return std::unique_ptr<File>(std::make_unique<SDL3File>(fp, start, end));
}

void SDL3SystemAPI::logInternal(
	LogLevel log_level, std::string tag, std::string msg) {
	SDL_LogMessage(
		SDL_LOG_CATEGORY_CUSTOM, getLogPriority(log_level), "[%s] %s",
		tag.c_str(), msg.c_str());
}

SDL_LogPriority SDL3SystemAPI::getLogPriority(LogLevel log_level) {
	switch (log_level) {
	case LogLevel::Debug:
		return SDL_LOG_PRIORITY_DEBUG;
	case LogLevel::Info:
		return SDL_LOG_PRIORITY_INFO;
	case LogLevel::Warn:
		return SDL_LOG_PRIORITY_WARN;
	case LogLevel::Error:
		return SDL_LOG_PRIORITY_ERROR;
	case LogLevel::Fatal:
		return SDL_LOG_PRIORITY_CRITICAL;
	}
	return SDL_LOG_PRIORITY_VERBOSE;
}

void SDL3SystemAPI::handleMouseEvent(SDL_Event& event) {
	if (inputProcessor) {
		int display_w, display_h, window_w, window_h;
		SDL_Window* window = SDL_GetWindowFromID(event.motion.windowID);
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GetWindowSizeInPixels(window, &display_w, &display_h);

		int x = event.motion.x;
		int y = event.motion.y;
		float scroll_x = 0;
		float scroll_y = 0;
		if (event.type == SDL_EVENT_MOUSE_WHEEL) {
			x = event.wheel.mouse_x;
			y = event.wheel.mouse_y;
			scroll_x = -event.wheel.x;
			scroll_y = event.wheel.y;
		}
		x *= display_w / (float)window_w;
		y *= display_h / (float)window_h;

#ifdef GROWL_IMGUI
		if (api.imguiVisible()) {
			int offset_x = 0;
			int offset_y = 0;
			imGuiGameWindowPos(&offset_x, &offset_y);
			x -= offset_x;
			y -= offset_y;
		}
#endif

		PointerEventType event_type = PointerEventType::Unknown;
		switch (event.type) {
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			event_type = PointerEventType::Down;
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			event_type = PointerEventType::Up;
			break;
		case SDL_EVENT_MOUSE_MOTION:
			event_type = PointerEventType::Move;
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			event_type = PointerEventType::Scroll;
			break;
		}

		MouseButton mouse_button = MouseButton::Unknown;
		switch (event.button.button) {
		case SDL_BUTTON_LEFT:
			mouse_button = MouseButton::LeftClick;
			break;
		case SDL_BUTTON_RIGHT:
			mouse_button = MouseButton::RightClick;
			break;
		case SDL_BUTTON_MIDDLE:
			mouse_button = MouseButton::MiddleClick;
			break;
		}

		InputEvent e(
			InputEventType::Mouse,
			InputMouseEvent{
				event_type, mouse_button, x, y, scroll_x, scroll_y});
		inputProcessor->onEvent(e);
	}
}
