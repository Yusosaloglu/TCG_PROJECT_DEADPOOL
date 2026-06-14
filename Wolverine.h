#pragma once
#include "Character.h"

class Wolverine : public Character {
public:
    Wolverine();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float clawExtend = 0.f;  // 0=retracted, 1=fully extended  (animated in SKILL1)
    float armAngle   = 0.f;  // degrees, arm raise during SKILL1

    void drawLegs();
    void drawTorso();
    void drawBelt();
    void drawArms();
    void drawClaws(int side);
    void drawHead();
};
