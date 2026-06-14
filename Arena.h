#pragma once
#include "platform_gl.h"
#include "Geometry.h"
#include "Constants.h"

// A single drifting grain/streak of wind-blown sand.
struct Dust {
    float x, y, z;
    float vx, vy, vz;     // small per-particle drift on top of the global wind
    float life, maxLife;
    float size;
};

class Arena {
public:
    float impactFlash = 0.f;  // seconds remaining; Game sets this on hit-land

    void init();   // generate textures + seed dust; call once after GL context is ready
    void tick(float dt);
    void draw();

    // Called each frame from Game::draw() before drawing anything.
    // t = 0 (all HP full) → 1 (both near dead).
    void setEnvironmentIntensity(float t);

    // Sky colours — interpolated by setEnvironmentIntensity, used in draw().
    Color3 skyTop = {0.65f, 0.65f, 0.70f};
    Color3 skyBot = {0.50f, 0.50f, 0.55f};

private:
    GLuint sandTex   = 0;
    GLuint cloudTex  = 0;
    float  envT      = 0.f;   // current environment intensity (0..1)

    Dust   dust[DUST_COUNT];
    float  windPhase = 0.f;   // advances time for gusts

    void drawGround();
    void drawSky();
    void drawRuins();
    void drawDust();

    void  seedDust();
    void  respawnDust(Dust& d, bool anywhere);

    static GLuint makeSandTexture(int N);
    static GLuint makeCloudTexture(int N);
};
