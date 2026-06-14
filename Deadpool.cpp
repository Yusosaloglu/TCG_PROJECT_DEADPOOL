#include "Deadpool.h"
#include "Geometry.h"
#include "Constants.h"
#include <cmath>

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
Deadpool::Deadpool() {
    x           =  2.f;
    z           =  0.f;
    facingAngle =  90.f;  // faces toward Wolverine's starting side
}

// ── Sub-draw routines ─────────────────────────────────────────────────────────

void Deadpool::drawLegs() {
    // Red upper legs
    glColor3f(0.80f, 0.10f, 0.10f);
    glPushMatrix(); glTranslatef(-0.17f, 0.45f, 0.f); drawBox(0.30f, 0.52f, 0.33f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.17f, 0.45f, 0.f); drawBox(0.30f, 0.52f, 0.33f); glPopMatrix();
    // Black lower legs / shin
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(-0.17f, 0.19f, 0.f); drawBox(0.27f, 0.30f, 0.31f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.17f, 0.19f, 0.f); drawBox(0.27f, 0.30f, 0.31f); glPopMatrix();
    // Red/black boots
    glColor3f(0.70f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(-0.17f, 0.07f, 0.f); drawBox(0.34f, 0.18f, 0.36f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.17f, 0.07f, 0.f); drawBox(0.34f, 0.18f, 0.36f); glPopMatrix();
}

void Deadpool::drawTorso() {
    // Red trapezoid torso
    glColor3f(0.80f, 0.10f, 0.10f);
    glPushMatrix(); glTranslatef(0.f, 0.72f, 0.f);
    drawTrapezoidBody(0.56f, 0.70f, 0.70f, 0.46f);
    glPopMatrix();
    // Black panel seam lines (thin overlay quads on front face)
    glColor3f(0.06f, 0.06f, 0.06f);
    glPushMatrix(); glTranslatef(0.f, 1.00f, 0.24f); drawBox(0.03f, 0.55f, 0.01f); glPopMatrix(); // centre seam
    glPushMatrix(); glTranslatef(0.f, 0.85f, 0.24f); drawBox(0.58f, 0.03f, 0.01f); glPopMatrix(); // horizontal seam
}

void Deadpool::drawBelt() {
    // Tan/gold belt
    glColor3f(0.78f, 0.66f, 0.38f);
    glPushMatrix(); glTranslatef(0.f, 0.73f, 0.f); drawBox(0.76f, 0.11f, 0.48f); glPopMatrix();
    // Belt buckle (square, slightly lighter)
    glColor3f(0.88f, 0.76f, 0.45f);
    glPushMatrix(); glTranslatef(0.f, 0.73f, 0.25f); drawBox(0.14f, 0.11f, 0.01f); glPopMatrix();
    // Side pouches
    glColor3f(0.70f, 0.58f, 0.30f);
    glPushMatrix(); glTranslatef(-0.40f, 0.73f, 0.f); drawBox(0.14f, 0.11f, 0.14f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.40f, 0.73f, 0.f); drawBox(0.14f, 0.11f, 0.14f); glPopMatrix();
    // Holster (right hip, darker)
    glColor3f(0.35f, 0.28f, 0.14f);
    glPushMatrix(); glTranslatef(0.40f, 0.55f, 0.12f); drawBox(0.10f, 0.28f, 0.10f); glPopMatrix();
}

void Deadpool::drawArms() {
    // Left arm (non-sword arm)
    glColor3f(0.80f, 0.10f, 0.10f);
    glPushMatrix(); glTranslatef(-0.52f, 1.06f, 0.f); drawBox(0.23f, 0.44f, 0.28f); glPopMatrix();
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(-0.52f, 0.68f, 0.f); drawBox(0.20f, 0.30f, 0.26f); glPopMatrix();

    // Right arm (sword arm) — rotated during skill
    glPushMatrix();
    glTranslatef(0.52f, 1.06f, 0.f);
    glRotatef(-kataAngle, 0.f, 0.f, 1.f);      // raise arm to the right
    glColor3f(0.80f, 0.10f, 0.10f);
    drawBox(0.23f, 0.44f, 0.28f);
    glColor3f(0.08f, 0.08f, 0.08f);
    glTranslatef(0.f, -0.38f, 0.f);
    drawBox(0.20f, 0.30f, 0.26f);
    // Katana attaches to sword hand
    drawKatana(kataAngle);
    glPopMatrix();
}

void Deadpool::drawKatana(float /*armLift*/) {
    glPushMatrix();
    glTranslatef(0.f, -0.50f, 0.f);  // below fist
    glRotatef(90.f, 0.f, 0.f, 1.f);  // blade points up in world space

    // Guard (black box)
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix(); drawBox(0.22f, 0.06f, 0.06f); glPopMatrix();

    // Blade (silver, specular metal)
    pushMetal();
    glColor3f(0.80f, 0.82f, 0.88f);
    glPushMatrix();
    glTranslatef(0.f, 0.48f, 0.f);
    drawBox(0.04f, 0.92f, 0.02f);   // flat blade
    glTranslatef(0.f, 0.50f, 0.f);
    drawWedgePrism(0.04f, 0.12f, 0.02f);  // tip
    glPopMatrix();
    popMetal();

    // Handle (red, gluCylinder is OK here — small decorative piece, not scored)
    glColor3f(0.70f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0.f, -0.22f, 0.f);
    drawBox(0.06f, 0.40f, 0.06f);   // fallback box handle so it works without quadric state
    glPopMatrix();

    glPopMatrix();
}

void Deadpool::drawHead() {
    glPushMatrix();
    glTranslatef(0.f, 1.72f, 0.f);

    // Main head volume: distinctive wide red cube (the iconic Deadpool cube-head)
    glColor3f(0.80f, 0.08f, 0.08f);
    drawBox(1.00f, 0.82f, 0.82f);

    // Black patches around eye areas
    glColor3f(0.06f, 0.06f, 0.06f);
    glPushMatrix(); glTranslatef(-0.22f, 0.06f, 0.42f); drawBox(0.30f, 0.26f, 0.02f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.22f, 0.06f, 0.42f); drawBox(0.30f, 0.26f, 0.02f); glPopMatrix();

    // White oval-ish lenses (approximated as wider flat boxes)
    glColor3f(0.94f, 0.96f, 0.98f);
    glPushMatrix(); glTranslatef(-0.22f, 0.06f, 0.43f); drawBox(0.22f, 0.18f, 0.01f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.22f, 0.06f, 0.43f); drawBox(0.22f, 0.18f, 0.01f); glPopMatrix();

    glPopMatrix();
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Deadpool::draw() {
    if (hidden) return;
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle, 0.f, 1.f, 0.f);

    drawLegs();
    drawTorso();
    drawBelt();
    drawArms();
    drawHead();

    glPopMatrix();
}

// ── Tick ──────────────────────────────────────────────────────────────────────
CharEvent Deadpool::tick(float dt, Character& other) {
    CharEvent ev;
    if (!alive) return ev;

    phaseTimer += dt;

    switch (phase) {
    // ── Skill 1: Katana Strike ────────────────────────────────────────────────
    case SKILL1_WINDUP:
        kataAngle = (phaseTimer / WINDUP_KATA) * 60.f;
        if (phaseTimer >= WINDUP_KATA) { phase = SKILL1_ACTIVE; phaseTimer = 0.f; }
        break;

    case SKILL1_ACTIVE:
        kataAngle = 60.f;
        if (!hitRegistered && inHitRange(other)) {
            other.applyDamage(DMG_KATANA);
            hitRegistered    = true;
            ev.hitLanded     = true;
            ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.55f, 0.1f, 20, 4.f);
        }
        if (phaseTimer >= ACTIVE_KATA) { phase = SKILL1_RECOVERY; phaseTimer = 0.f; }
        break;

    case SKILL1_RECOVERY:
        kataAngle = 60.f * (1.f - phaseTimer / RECOV_KATA);
        if (phaseTimer >= RECOV_KATA) { phase = IDLE; phaseTimer = 0.f; kataAngle = 0.f; }
        break;

    // ── Skill 2: Teleport Blink ───────────────────────────────────────────────
    // No alpha fade (avoids translucency sort artifacts in fixed-function GL).
    // Instead: model disappears, grey puff at old pos, reappears behind opponent.
    case SKILL2_WINDUP:
        hidden    = true;
        blinkDone = false;
        // Grey puff at departure point (emitted once on first frame)
        if (phaseTimer <= dt)
            ev.addEmit(x, 1.f, z, 0.6f, 0.6f, 0.6f, 14, 2.5f);
        if (phaseTimer >= BLINK_HIDE) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;

    case SKILL2_ACTIVE:
        if (!blinkDone) {
            blinkDone = true;
            // Reposition to 1.8 units behind opponent
            float bx = other.x - sinf(other.facingAngle * (float)M_PI / 180.f) * 1.8f;
            float bz = other.z - cosf(other.facingAngle * (float)M_PI / 180.f) * 1.8f;
            x = bx; z = bz;
            // Face toward the opponent's back
            facingAngle = other.facingAngle + 180.f;
            hidden = false;
            // Arrival puff + backstab
            ev.addEmit(x, 1.f, z, 0.6f, 0.6f, 0.6f, 14, 2.5f);
            other.applyDamage(DMG_BLINK);
            hitRegistered    = true;
            ev.hitLanded     = true;
            ev.doImpactFlash = true;
        }
        phase = SKILL2_RECOVERY; phaseTimer = 0.f;
        break;

    case SKILL2_RECOVERY:
        hidden = false;
        if (phaseTimer >= RECOV_BLINK) { phase = IDLE; phaseTimer = 0.f; }
        break;

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;

    default: break;
    }

    return ev;
}
