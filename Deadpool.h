#pragma once
#include "Character.h"

class Deadpool : public Character {
public:
    Deadpool();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float kataAngle = 0.f;   // katana arm raise angle, degrees (SKILL1)
    bool  blinkDone = false; // true once teleport repositioning has happened

    void drawLegs();
    void drawTorso();
    void drawBelt();
    void drawArms();
    void drawKatana(float armLift);
    void drawHead();
};
