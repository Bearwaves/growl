#ifdef GROWL_IMGUI

#pragma once

namespace Growl {

void imGuiBegin();
void imGuiBeginGameWindow();
void imGuiEndGameWindow();
bool imGuiGameWindowFocused();
void imGuiGameWindowSize(int* w, int* h);
void imGuiGameWindowPos(int* x, int* y);
void imGuiEnd();

namespace {
bool game_window_focused;
int game_window_x;
int game_window_y;
} // namespace
} // namespace Growl

#endif
