#pragma once
#include "Character.h"

class Wolverine : public Character {
public:
    Wolverine();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float clawExtend = 0.f;  // 0 = hidden in fist, 1 = fully extended
    float armPitch   = 0.f;  // degrees: arm raise about X for Skill 1 up-slash
    float armSpread  = 0.f;  // degrees: arms swung out to the sides for Skill 2 spin

    void drawLegs();
    void drawTorso();
    void drawArm(int side);   // upper + forearm + glove + claws, in hand-local space
    void drawClaws();         // 3 blades, drawn in glove-local space
    void drawHead();
};
