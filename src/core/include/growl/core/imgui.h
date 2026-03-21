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

} // namespace Growl
#endif
