#pragma once
#include "platform_gl.h"
#include "Constants.h"

// ── Emit request (one particle burst) ─────────────────────────────────────────
struct EmitRequest {
    float x, y, z;
    float r, g, b;
    int   count;
    float speed;
};

// ── Event returned by Character::tick() each frame ────────────────────────────
// Characters stay decoupled from ParticleSystem and Arena; Game consumes events.
struct CharEvent {
    bool  hitLanded     = false;
    bool  doImpactFlash = false;

    static const int MAX_EMITS = 4;  // covers blink (2 puffs), slash, heal motes
    EmitRequest emits[MAX_EMITS];
    int emitCount = 0;

    void addEmit(float x, float y, float z,
                 float r, float g, float b, int n, float spd) {
        if (emitCount < MAX_EMITS)
            emits[emitCount++] = {x,y,z,r,g,b,n,spd};
    }
};

// ── Abstract character base ────────────────────────────────────────────────────
class Character {
public:
    // World position (y=0 is ground level; Y is up)
    float x = 0.f, z = 0.f;
    float facingAngle = 0.f;  // Y-axis rotation, degrees; 0 = facing +z

    float hp    = MAX_HP;
    bool  alive = true;

    bool  hitRegistered   = false;  // guard: only one hit per skill window

    enum Phase {
        IDLE,
        SKILL1_WINDUP, SKILL1_ACTIVE, SKILL1_RECOVERY,
        SKILL2_WINDUP, SKILL2_ACTIVE, SKILL2_RECOVERY,
        HIT_STUN, DEAD
    };
    Phase phase      = IDLE;
    float phaseTimer = 0.f;   // seconds elapsed in current phase

    // Dodge roll — independent of the skill phase (see startRoll / updateRoll)
    float rollTimer = 0.f;    // counts down from ROLL_DUR while rolling
    float rollAngle = 0.f;    // current tumble angle (degrees), applied in draw()
    int   rollDir   = 0;      // +1 = roll left, -1 = roll right
    bool  rolling() const { return rollTimer > 0.f; }

    virtual ~Character() = default;

    // Subclasses implement geometry and skill logic
    virtual void      draw()                             = 0;
    virtual CharEvent tick(float dt, Character& other)   = 0;

    // Called from Game::keyDown — safe: guards phase==IDLE internally
    void         startSkill1();
    virtual void startSkill2();   // Wolverine overrides to gate on the heal gauge

    // Dodge roll: dir +1 = left, -1 = right.  Starts only from IDLE.
    void startRoll(int dir);
    void updateRoll(float dt);    // advances the roll + lateral move; called by Game

    void applyDamage(float dmg);

    // True if 'other' is within HIT_RANGE and inside the facing half-cone.
    // Handles angle wraparound.  Used by the directional slash skills.
    bool inHitRange(const Character& other) const;

    // General forward-cone test: 'other' within `range` and inside the
    // ±`tolDeg` half-cone about facing.  Handles wraparound.  Backs both
    // inHitRange (melee) and the Desert Eagle hitscan.
    bool inCone(const Character& other, float range, float tolDeg) const;
};
