#pragma once

#ifdef GROWL_OPENGL_3_3
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#elif GROWL_OPENGL_4_5
#include "../thirdparty/opengl_4_5.h"
#endif
