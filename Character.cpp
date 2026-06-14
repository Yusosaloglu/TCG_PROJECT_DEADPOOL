#include "Character.h"
#include <cmath>

void Character::startSkill1() {
    if (phase != IDLE || rolling()) return;
    phase = SKILL1_WINDUP;
    phaseTimer    = 0.f;
    hitRegistered = false;
}

void Character::startSkill2() {
    if (phase != IDLE || rolling()) return;
    phase = SKILL2_WINDUP;
    phaseTimer    = 0.f;
    hitRegistered = false;
}

// ── Dodge roll ────────────────────────────────────────────────────────────────
void Character::startRoll(int dir) {
    if (phase != IDLE || rolling() || !alive) return;
    rollTimer = ROLL_DUR;
    rollDir   = dir;          // +1 = left, -1 = right
    rollAngle = 0.f;
}

// Advances the lateral roll each frame: moves perpendicular to facing and spins
// the tumble angle.  Position is clamped to the arena.  Called from Game::tick.
void Character::updateRoll(float dt) {
    if (rollTimer <= 0.f) return;
    rollTimer -= dt;

    float t = 1.f - rollTimer / ROLL_DUR;        // 0 → 1 progress
    if (t > 1.f) t = 1.f;
    rollAngle = rollDir * 360.f * t;             // one full tumble about the forward axis

    float rad = facingAngle * (float)M_PI / 180.f;
    float lx  = cosf(rad), lz = -sinf(rad);      // "left" basis (matches the old strafe)
    float step = ROLL_SPEED * dt;
    x += rollDir * lx * step;
    z += rollDir * lz * step;

    if (x < -ARENA_HALF) x = -ARENA_HALF; else if (x > ARENA_HALF) x = ARENA_HALF;
    if (z < -ARENA_HALF) z = -ARENA_HALF; else if (z > ARENA_HALF) z = ARENA_HALF;

    if (rollTimer <= 0.f) { rollTimer = 0.f; rollAngle = 0.f; }
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
