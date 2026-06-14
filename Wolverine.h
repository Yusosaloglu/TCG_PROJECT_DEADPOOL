#pragma once
#include "Character.h"

class Wolverine : public Character {
public:
    Wolverine();
    void      draw()                           override;
    CharEvent tick(float dt, Character& other) override;
    void      startSkill2()                    override;  // gated on the heal gauge

    // Berserker Heal gauge in HP points; public so the HUD can show it via Game.
    float healGauge = 0.f;

private:
    float clawExtend = 0.f;  // 0 = hidden in fist, 1 = fully extended
    float armPitch   = 0.f;  // degrees: arm raise about X for Skill 1 up-slash
    float healBanked = 0.f;  // gauge snapshot taken when the heal becomes ACTIVE

    void drawLegs();
    void drawTorso();
    void drawArm(int side);   // upper + forearm + glove + claws, in hand-local space
    void drawClaws();         // 3 blades, drawn in glove-local space
    void drawHead();
    void drawHealAura();      // translucent green glow during the heal phases
};
