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
    float dx   = other.x - x;
    float dz   = other.z - z;
    float dist = sqrtf(dx*dx + dz*dz);
    if (dist > HIT_RANGE) return false;

    float angleToTarget = atan2f(dx, dz) * 180.f / (float)M_PI;
    float diff = angleToTarget - facingAngle;
    while (diff >  180.f) diff -= 360.f;
    while (diff < -180.f) diff += 360.f;
    return fabsf(diff) < HIT_ANGLE_TOL;
}

bool Character::inRange(const Character& other) const {
    float dx = other.x - x;
    float dz = other.z - z;
    return (dx*dx + dz*dz) < (HIT_RANGE * HIT_RANGE);
}
