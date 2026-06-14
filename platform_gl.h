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
