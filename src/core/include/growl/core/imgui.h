#ifdef GROWL_IMGUI

#pragma once

namespace Growl {

class API;

void imGuiBegin(API& api);
void imGuiBeginGameWindow();
void imGuiEndGameWindow();
bool imGuiGameWindowFocused();
void imGuiGameWindowSize(int* w, int* h);
void imGuiGameWindowPos(int* x, int* y);
bool imGuiGameWindowResized();
void imGuiEnd();

namespace {
bool game_window_focused;
int game_window_x;
int game_window_y;
int game_window_w = 0;
int game_window_h = 0;
bool game_window_resized;
bool system_api_view = false;
bool window_view = false;
} // namespace
} // namespace Growl

#endif
