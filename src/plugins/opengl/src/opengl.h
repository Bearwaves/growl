#pragma once

#ifdef __APPLE__
#define GROWL_OPENGL_APPLE
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#endif
