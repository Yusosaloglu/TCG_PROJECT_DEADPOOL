#pragma once

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>   // freeglut on Windows — also pulls in gl.h and glu.h
#endif

#include <cmath>
#include <cstring>
#include <algorithm>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

// MinGW's legacy <GL/gl.h> only exposes OpenGL 1.1 tokens. Provide the
// OpenGL 1.2 GL_CLAMP_TO_EDGE constant if the platform header omits it.
#ifndef GL_CLAMP_TO_EDGE
  #define GL_CLAMP_TO_EDGE 0x812F
#endif
