#ifdef GROWL_IMGUI

#pragma once

namespace Growl {

class API;

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
void imGuiDockLeft(const char* name);
void imGuiDockRight(const char* name);
void imGuiDockBottom(const char* name);

} // namespace Growl
#endif
