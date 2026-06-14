#pragma once
#include "Character.h"

class Deadpool : public Character {
public:
    Deadpool();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float kataYaw     = 0.f;  // degrees: horizontal left->right sweep for Skill 1 slash

    // Desert Eagle tracer beam (world-space endpoints, drawn after the body matrix)
    float tracerTimer = 0.f;
    float tracerSx = 0.f, tracerSy = 0.f, tracerSz = 0.f;
    float tracerEx = 0.f, tracerEy = 0.f, tracerEz = 0.f;

    void drawLegs();
    void drawTorso();
    void drawBelt();
    void drawBackGear();     // X-harness + twin scabbards on the back
    void drawArms();
    // uprightDeg: rotation that stands the blade up (idle); extend: blade points
    // out along the arm (slash) instead of up.
    void drawKatana(float uprightDeg, bool extend);
    void drawDesertEagle();  // hand-built pistol, held during the gun skill
    void drawTracer();       // additive shot beam in world space
    void drawHead();
};
