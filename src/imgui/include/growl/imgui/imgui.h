#ifdef GROWL_IMGUI

#pragma once

namespace Growl {

class API;

enum class ImGuiDockDirection { None, Left, Right, Bottom };

void imGuiSetup();
void imGuiBegin(API& api);
void imGuiBeginGameWindow();
void imGuiEndGameWindow();
bool imGuiGameWindowFocused();
void imGuiFocusGameWindow();
void imGuiGameWindowSize(int* w, int* h);
void imGuiGameWindowPos(int* x, int* y);
bool imGuiGameWindowResized();
void imGuiEnd();
void imGuiWindow(
	const char* name, bool default_open,
	ImGuiDockDirection default_dock_direction = ImGuiDockDirection::None);
void imGuiDockLeft(const char* name);
void imGuiDockRight(const char* name);
void imGuiDockBottom(const char* name);

} // namespace Growl
#endif
