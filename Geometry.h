#pragma once
#include "platform_gl.h"

// ──────────────────────────────────────────────────────────────────────────────
// All functions are inline to avoid multiple-definition linker errors.
// Without inline, each .cpp that includes this header produces its own
// drawBox() symbol and MinGW fails at link time.
//
// All draw functions:
//   • Draw at the local origin
//   • Caller wraps with glPushMatrix / glPopMatrix + transforms
//   • Set glNormal3f before every face (required for GL_LIGHTING)
//   • glColor3f must be set by caller before the call
// ──────────────────────────────────────────────────────────────────────────────

// ── Math helpers ──────────────────────────────────────────────────────────────

inline float lerpf(float a, float b, float t)  { return a + (b - a) * t; }
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : v > hi ? hi : v; }

struct Color3 { float r, g, b; };
inline Color3 lerpColor(Color3 a, Color3 b, float t) {
    return { lerpf(a.r, b.r, t), lerpf(a.g, b.g, t), lerpf(a.b, b.b, t) };
}

// ── Primitives ────────────────────────────────────────────────────────────────

// Axis-aligned box, centred at origin, dimensions w × h × d.
inline void drawBox(float w, float h, float d) {
    float hw = w * .5f, hh = h * .5f, hd = d * .5f;
    // Front  +z
    glNormal3f(0, 0, 1);  glBegin(GL_QUADS);
    glVertex3f(-hw,-hh, hd); glVertex3f( hw,-hh, hd);
    glVertex3f( hw, hh, hd); glVertex3f(-hw, hh, hd); glEnd();
    // Back  -z
    glNormal3f(0, 0,-1);  glBegin(GL_QUADS);
    glVertex3f( hw,-hh,-hd); glVertex3f(-hw,-hh,-hd);
    glVertex3f(-hw, hh,-hd); glVertex3f( hw, hh,-hd); glEnd();
    // Right +x
    glNormal3f(1, 0, 0);  glBegin(GL_QUADS);
    glVertex3f( hw,-hh, hd); glVertex3f( hw,-hh,-hd);
    glVertex3f( hw, hh,-hd); glVertex3f( hw, hh, hd); glEnd();
    // Left  -x
    glNormal3f(-1, 0, 0); glBegin(GL_QUADS);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh, hd);
    glVertex3f(-hw, hh, hd); glVertex3f(-hw, hh,-hd); glEnd();
    // Top   +y
    glNormal3f(0, 1, 0);  glBegin(GL_QUADS);
    glVertex3f(-hw, hh, hd); glVertex3f( hw, hh, hd);
    glVertex3f( hw, hh,-hd); glVertex3f(-hw, hh,-hd); glEnd();
    // Bottom -y
    glNormal3f(0,-1, 0);  glBegin(GL_QUADS);
    glVertex3f(-hw,-hh,-hd); glVertex3f( hw,-hh,-hd);
    glVertex3f( hw,-hh, hd); glVertex3f(-hw,-hh, hd); glEnd();
}

// Trapezoid body (torso shape): wider at bottom (botW) than top (topW).
// Bottom at y=0, top at y=h.  Depth d is uniform.
inline void drawTrapezoidBody(float topW, float botW, float h, float d) {
    float htw = topW * .5f, hbw = botW * .5f, hd = d * .5f;

    // Front +z  (planar in XY so normal is simply (0,0,1))
    glNormal3f(0, 0, 1); glBegin(GL_QUADS);
    glVertex3f(-hbw, 0, hd); glVertex3f( hbw, 0, hd);
    glVertex3f( htw, h, hd); glVertex3f(-htw, h, hd); glEnd();
    // Back -z
    glNormal3f(0, 0,-1); glBegin(GL_QUADS);
    glVertex3f( hbw, 0,-hd); glVertex3f(-hbw, 0,-hd);
    glVertex3f(-htw, h,-hd); glVertex3f( htw, h,-hd); glEnd();
    // Right slant: normal = (h, hbw-htw, 0) — GL_NORMALIZE will unit-ize it
    { float nx = h, ny = hbw - htw, len = sqrtf(nx*nx + ny*ny);
      if (len > 0) glNormal3f(nx/len, ny/len, 0); else glNormal3f(1,0,0);
      glBegin(GL_QUADS);
      glVertex3f( hbw, 0, hd); glVertex3f( hbw, 0,-hd);
      glVertex3f( htw, h,-hd); glVertex3f( htw, h, hd); glEnd(); }
    // Left slant: normal = (-h, hbw-htw, 0)
    { float nx = -h, ny = hbw - htw, len = sqrtf(nx*nx + ny*ny);
      if (len > 0) glNormal3f(nx/len, ny/len, 0); else glNormal3f(-1,0,0);
      glBegin(GL_QUADS);
      glVertex3f(-hbw, 0,-hd); glVertex3f(-hbw, 0, hd);
      glVertex3f(-htw, h, hd); glVertex3f(-htw, h,-hd); glEnd(); }
    // Top
    glNormal3f(0, 1, 0); glBegin(GL_QUADS);
    glVertex3f(-htw, h, hd); glVertex3f( htw, h, hd);
    glVertex3f( htw, h,-hd); glVertex3f(-htw, h,-hd); glEnd();
    // Bottom
    glNormal3f(0,-1, 0); glBegin(GL_QUADS);
    glVertex3f(-hbw, 0,-hd); glVertex3f( hbw, 0,-hd);
    glVertex3f( hbw, 0, hd); glVertex3f(-hbw, 0, hd); glEnd();
}

// Triangular prism with tip at top-centre (0, h, 0) and base at y=0.
// Used for Wolverine hair spikes and katana blade tip.
inline void drawWedgePrism(float w, float h, float d) {
    float hw = w * .5f, hd = d * .5f;

    // Front triangle +z
    glNormal3f(0, 0, 1); glBegin(GL_TRIANGLES);
    glVertex3f(-hw, 0, hd); glVertex3f( hw, 0, hd); glVertex3f(0, h, hd); glEnd();
    // Back triangle -z
    glNormal3f(0, 0,-1); glBegin(GL_TRIANGLES);
    glVertex3f( hw, 0,-hd); glVertex3f(-hw, 0,-hd); glVertex3f(0, h,-hd); glEnd();
    // Bottom
    glNormal3f(0,-1, 0); glBegin(GL_QUADS);
    glVertex3f(-hw, 0,-hd); glVertex3f( hw, 0,-hd);
    glVertex3f( hw, 0, hd); glVertex3f(-hw, 0, hd); glEnd();
    // Right slant: normal = (h, hw, 0) normalised
    { float nx = h, ny = hw, len = sqrtf(nx*nx + ny*ny);
      glNormal3f(nx/len, ny/len, 0); glBegin(GL_QUADS);
      glVertex3f( hw, 0, hd); glVertex3f( hw, 0,-hd);
      glVertex3f(  0, h,-hd); glVertex3f(  0, h, hd); glEnd(); }
    // Left slant: normal = (-h, hw, 0) normalised
    { float nx = -h, ny = hw, len = sqrtf(nx*nx + ny*ny);
      glNormal3f(nx/len, ny/len, 0); glBegin(GL_QUADS);
      glVertex3f(  0, h, hd); glVertex3f(  0, h,-hd);
      glVertex3f(-hw, 0,-hd); glVertex3f(-hw, 0, hd); glEnd(); }
}

// Horizontal quad in the XZ plane (y=0), w × d.  Used for ground tiles, shadows.
inline void drawFlatQuad(float w, float d) {
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glVertex3f(-w*.5f, 0,-d*.5f); glVertex3f( w*.5f, 0,-d*.5f);
    glVertex3f( w*.5f, 0, d*.5f); glVertex3f(-w*.5f, 0, d*.5f);
    glEnd();
}

// Tiled ground plane: 2·size × 2·size centred at origin, tiles×tiles grid.
// Caller must bind texture and enable GL_TEXTURE_2D before this call.
inline void drawGroundPlane(float size, int tiles) {
    float step  = size * 2.f / tiles;
    float uStep = 1.f / tiles;
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int tz = 0; tz < tiles; ++tz) {
        for (int tx = 0; tx < tiles; ++tx) {
            float x0 = -size + tx * step, z0 = -size + tz * step;
            float u0 = tx * uStep,        v0 = tz * uStep;
            glTexCoord2f(u0,        v0       ); glVertex3f(x0,       0, z0       );
            glTexCoord2f(u0+uStep,  v0       ); glVertex3f(x0+step,  0, z0       );
            glTexCoord2f(u0+uStep,  v0+uStep ); glVertex3f(x0+step,  0, z0+step  );
            glTexCoord2f(u0,        v0+uStep ); glVertex3f(x0,       0, z0+step  );
        }
    }
    glEnd();
}
