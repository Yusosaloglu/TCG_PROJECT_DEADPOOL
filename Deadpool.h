#pragma once
#include "Character.h"

class Deadpool : public Character {
public:
    Deadpool();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;

private:
    float kataSwing = 0.f;   // degrees: sword-arm forward swing for Skill 1 slash
    float armSpread = 0.f;   // degrees: arms swung out to the sides for Skill 2 spin

    void drawLegs();
    void drawTorso();
    void drawBelt();
    void drawArms();
    void drawKatana();       // drawn in sword-hand-local space
    void drawHead();
};
