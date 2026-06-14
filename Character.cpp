#include "Character.h"
#include <cmath>

void Character::startSkill1() {
    if (phase != IDLE) return;
    phase = SKILL1_WINDUP;
    phaseTimer    = 0.f;
    hitRegistered = false;
}

void Character::startSkill2() {
    if (phase != IDLE) return;
    phase = SKILL2_WINDUP;
    phaseTimer    = 0.f;
    hitRegistered = false;
}

void Character::applyDamage(float dmg) {
    hp -= dmg;
    if (hp <= 0.f) {
        hp    = 0.f;
        alive = false;
        phase = DEAD;
    }
}

bool Character::inHitRange(const Character& other) const {
    return inCone(other, HIT_RANGE, HIT_ANGLE_TOL);
}

// General forward-cone hit test, shared by the melee slashes (short range,
// wide cone) and the Desert Eagle hitscan (long range, narrow cone).
bool Character::inCone(const Character& other, float range, float tolDeg) const {
    float dx   = other.x - x;
    float dz   = other.z - z;
    float dist = sqrtf(dx*dx + dz*dz);
    if (dist > range) return false;

    float angleToTarget = atan2f(dx, dz) * 180.f / (float)M_PI;
    float diff = angleToTarget - facingAngle;
    while (diff >  180.f) diff -= 360.f;
    while (diff < -180.f) diff += 360.f;
    return fabsf(diff) < tolDeg;
}
