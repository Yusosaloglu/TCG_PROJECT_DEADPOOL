#include "Wolverine.h"
#include "Geometry.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

// ── Metal specular helpers ─────────────────────────────────────────────────────
static void pushMetal() {
    static const GLfloat spec[]  = {0.9f, 0.9f, 0.9f, 1.f};
    static const GLfloat shine[] = {64.f};
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shine);
}
static void popMetal() {
    static const GLfloat nospec[]  = {0.f, 0.f, 0.f, 1.f};
    static const GLfloat noshine[] = {0.f};
    glMaterialfv(GL_FRONT, GL_SPECULAR,  nospec);
    glMaterialfv(GL_FRONT, GL_SHININESS, noshine);
}

// ── Constructor ───────────────────────────────────────────────────────────────
Wolverine::Wolverine() {
    x            = -2.f;
    z            =  0.f;
    facingAngle  = -90.f;  // faces toward Deadpool's starting side
}

// ── Sub-draw routines (called from draw(), all in local character space) ──────

void Wolverine::drawLegs() {
    // Yellow thighs
    glColor3f(0.90f, 0.75f, 0.10f);
    glPushMatrix(); glTranslatef(-0.18f, 0.45f, 0.f); drawBox(0.32f, 0.55f, 0.34f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.18f, 0.45f, 0.f); drawBox(0.32f, 0.55f, 0.34f); glPopMatrix();
    // Dark blue boots
    glColor3f(0.10f, 0.15f, 0.50f);
    glPushMatrix(); glTranslatef(-0.18f, 0.10f, 0.f); drawBox(0.36f, 0.22f, 0.38f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.18f, 0.10f, 0.f); drawBox(0.36f, 0.22f, 0.38f); glPopMatrix();
}

void Wolverine::drawTorso() {
    // Yellow trapezoid body (wider at hips)
    glColor3f(0.90f, 0.75f, 0.10f);
    glPushMatrix(); glTranslatef(0.f, 0.72f, 0.f);
    drawTrapezoidBody(0.58f, 0.72f, 0.72f, 0.48f);
    glPopMatrix();
    // Dark blue chest stripe (front overlay)
    glColor3f(0.10f, 0.15f, 0.50f);
    glPushMatrix(); glTranslatef(0.f, 1.10f, 0.25f); drawBox(0.62f, 0.28f, 0.02f); glPopMatrix();
}

void Wolverine::drawBelt() {
    // Brown belt
    glColor3f(0.48f, 0.30f, 0.10f);
    glPushMatrix(); glTranslatef(0.f, 0.73f, 0.f); drawBox(0.78f, 0.11f, 0.50f); glPopMatrix();
    // Gold X-buckle
    glColor3f(0.88f, 0.72f, 0.12f);
    glPushMatrix(); glTranslatef(0.f, 0.73f, 0.26f); drawBox(0.16f, 0.11f, 0.02f); glPopMatrix();
}

void Wolverine::drawArms() {
    for (int s = -1; s <= 1; s += 2) {
        float sx = s * 0.54f;
        // Dark blue upper arm
        glColor3f(0.10f, 0.15f, 0.50f);
        glPushMatrix();
        glTranslatef(sx, 1.08f, 0.f);
        glRotatef(s * armAngle, 0.f, 0.f, 1.f);   // raise arm during skill
        drawBox(0.24f, 0.46f, 0.30f);
        // Yellow forearm (child of arm so it follows the rotation)
        glColor3f(0.90f, 0.75f, 0.10f);
        glTranslatef(0.f, -0.40f, 0.f);
        drawBox(0.21f, 0.38f, 0.27f);
        // Dark blue glove
        glColor3f(0.10f, 0.15f, 0.50f);
        glTranslatef(0.f, -0.32f, 0.f);
        drawBox(0.20f, 0.20f, 0.26f);
        glPopMatrix();
    }
}

void Wolverine::drawClaws(int side) {
    // side: -1=left, +1=right
    float sx      = side * 0.54f;
    float baseLen = 0.28f;
    float ext     = baseLen + clawExtend * 0.24f;  // extend forward during SKILL1_ACTIVE

    pushMetal();
    glColor3f(0.82f, 0.84f, 0.90f);
    for (int c = -1; c <= 1; ++c) {
        glPushMatrix();
        // Attach to fist position
        glTranslatef(sx + c * 0.055f, 0.62f, 0.24f);
        glRotatef(-75.f, 1.f, 0.f, 0.f);  // point forward/downward
        drawWedgePrism(0.038f, ext, 0.038f);
        glPopMatrix();
    }
    popMetal();
}

void Wolverine::drawHead() {
    glPushMatrix();
    glTranslatef(0.f, 1.72f, 0.f);

    // Large black cowl (dominates head volume)
    glColor3f(0.06f, 0.06f, 0.06f);
    drawBox(0.88f, 0.82f, 0.72f);

    // Two pointed hair spikes at top-sides
    glColor3f(0.06f, 0.06f, 0.06f);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(s * 0.26f, 0.56f, 0.f);
        drawWedgePrism(0.22f, 0.44f, 0.20f);
        glPopMatrix();
    }

    // Skin face inset (visible only from the front)
    glColor3f(0.94f, 0.80f, 0.70f);
    glPushMatrix(); glTranslatef(0.f, -0.04f, 0.37f);
    drawBox(0.44f, 0.36f, 0.02f); glPopMatrix();

    // Tiny dark eyes
    glColor3f(0.06f, 0.06f, 0.06f);
    glPushMatrix(); glTranslatef(-0.10f, 0.02f, 0.38f); drawBox(0.07f, 0.07f, 0.01f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.10f, 0.02f, 0.38f); drawBox(0.07f, 0.07f, 0.01f); glPopMatrix();

    glPopMatrix();
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Wolverine::draw() {
    if (hidden) return;
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle, 0.f, 1.f, 0.f);

    drawLegs();
    drawTorso();
    drawBelt();
    drawArms();
    drawClaws(-1);
    drawClaws( 1);
    drawHead();

    glPopMatrix();
}

// ── Tick ──────────────────────────────────────────────────────────────────────
CharEvent Wolverine::tick(float dt, Character& other) {
    CharEvent ev;
    if (!alive) return ev;

    phaseTimer += dt;

    switch (phase) {
    // ── Skill 1: Berserker Slash ──────────────────────────────────────────────
    case SKILL1_WINDUP:
        armAngle   = (phaseTimer / WINDUP_SLASH) * 50.f;
        clawExtend = 0.f;
        if (phaseTimer >= WINDUP_SLASH) { phase = SKILL1_ACTIVE; phaseTimer = 0.f; }
        break;

    case SKILL1_ACTIVE:
        armAngle   = 50.f;
        clawExtend = 1.f;
        if (!hitRegistered && inHitRange(other)) {
            other.applyDamage(DMG_SLASH);
            hitRegistered      = true;
            ev.hitLanded       = true;
            ev.doImpactFlash   = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.9f, 0.2f, 20, 4.f);
        }
        if (phaseTimer >= ACTIVE_SLASH) { phase = SKILL1_RECOVERY; phaseTimer = 0.f; }
        break;

    case SKILL1_RECOVERY:
        { float t = phaseTimer / RECOV_SLASH;
          armAngle   = 50.f * (1.f - t);
          clawExtend = 1.f  * (1.f - t); }
        if (phaseTimer >= RECOV_SLASH) {
            phase = IDLE; phaseTimer = 0.f; armAngle = 0.f; clawExtend = 0.f;
        }
        break;

    // ── Skill 2: Healing Factor ───────────────────────────────────────────────
    case SKILL2_WINDUP:
        if (phaseTimer >= WINDUP_HEAL) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;

    case SKILL2_ACTIVE:
        hp = std::min(hp + HEAL_RATE * dt, MAX_HP);
        // Emit green motes periodically (every ~0.1 s)
        if ((int)(phaseTimer / 0.1f) > (int)((phaseTimer - dt) / 0.1f))
            ev.addEmit(x, 1.0f, z, 0.2f, 0.9f, 0.3f, 6, 1.5f);
        if (phaseTimer >= ACTIVE_HEAL) { phase = SKILL2_RECOVERY; phaseTimer = 0.f; }
        break;

    case SKILL2_RECOVERY:
        if (phaseTimer >= RECOV_HEAL) { phase = IDLE; phaseTimer = 0.f; }
        break;

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;

    default: break;
    }

    return ev;
}
