#pragma once
#include "Character.h"

class Deadpool : public Character {
public:
    Deadpool();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float kataYaw   = 0.f;   // degrees: horizontal left->right sweep for Skill 1 slash
    float armSpread = 0.f;   // degrees: arms swung out to the sides for Skill 2 spin

    void drawLegs();
    void drawTorso();
    void drawBelt();
    void drawBackGear();     // X-harness + twin scabbards on the back
    void drawArms();
    // uprightDeg: rotation that stands the blade up (idle); extend: blade points
    // out along the arm (slash/spin) instead of up.
    void drawKatana(float uprightDeg, bool extend);
    void drawHead();
};
