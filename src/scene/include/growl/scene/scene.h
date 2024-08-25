#pragma once

#include "growl/core/api/api.h"

namespace Growl {

Error initSceneGraph(API& api);

#ifdef GROWL_IMGUI
void imGuiBeginSceneWindow();
void imGuiEndSceneWindow();
#endif

} // namespace Growl
