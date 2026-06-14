#pragma once
#include "platform_gl.h"
#include "Geometry.h"

class Arena {
public:
    float impactFlash = 0.f;  // seconds remaining; Game sets this on hit-land

    void init();   // generate sand texture; call once after GL context is ready
    void tick(float dt);
    void draw();

    // Called each frame from Game::draw() before drawing anything.
    // t = 0 (all HP full) → 1 (both near dead).
    void setEnvironmentIntensity(float t);

    // Sky colours — interpolated by setEnvironmentIntensity, used in draw().
    Color3 skyTop = {0.65f, 0.65f, 0.70f};
    Color3 skyBot = {0.50f, 0.50f, 0.55f};

private:
    GLuint sandTex = 0;
    float  stormRate = 0.f;  // dust particles per tick (driven by intensity)

    void drawGround();
    void drawSky();
    void drawRuins();

    static GLuint makeSandTexture(int N);
};
