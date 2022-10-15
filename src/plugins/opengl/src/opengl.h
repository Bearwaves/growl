#pragma once

#ifdef GROWL_OPENGL_4_1
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#elif GROWL_OPENGL_4_5
#include "../thirdparty/opengl_4_5.h"
#elif GROWL_OPENGL_ES
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#endif
