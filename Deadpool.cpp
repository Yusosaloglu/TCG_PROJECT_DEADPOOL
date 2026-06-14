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

// Palette
static const float RED[3]   = {0.78f, 0.10f, 0.10f};
static const float BLACK[3] = {0.10f, 0.10f, 0.11f};
static const float TAN[3]   = {0.62f, 0.50f, 0.28f};

// ── Constructor ───────────────────────────────────────────────────────────────
Deadpool::Deadpool() {
    x           =  2.f;
    z           =  0.f;
    facingAngle =  90.f;  // face toward Wolverine's side
    kataSwing   =  KATA_IDLE_PITCH;
}

// ── Legs ──────────────────────────────────────────────────────────────────────
void Deadpool::drawLegs() {
    glColor3fv(RED);                                    // red thighs
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.42f, 0.f); drawBox(0.30f, 0.50f, 0.32f); glPopMatrix();
    }
    glColor3fv(BLACK);                                  // black knee bands
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.26f, 0.f); drawBox(0.31f, 0.12f, 0.33f); glPopMatrix();
    }
    glColor3fv(RED);                                    // red boots + black toes
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.13f, 0.02f); drawBox(0.34f, 0.26f, 0.40f); glPopMatrix();
        glColor3fv(BLACK);
        glPushMatrix(); glTranslatef(s*0.17f, 0.10f, 0.24f); glScalef(1.f, 0.8f, 1.f); drawSphere(0.14f, 12, 10); glPopMatrix();
        glColor3fv(RED);
    }
}

// ── Torso (red body, black shoulders + side panels, centre seam) ───────────────
void Deadpool::drawTorso() {
    glColor3fv(RED);
    glPushMatrix(); glTranslatef(0.f, 0.62f, 0.f); drawTrapezoidBody(0.54f, 0.66f, 0.56f, 0.42f); glPopMatrix();

    glColor3fv(BLACK);                                  // black shoulder caps
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.34f, 1.12f, 0.f); drawSphere(0.20f, 14, 12); glPopMatrix();
    }
    glColor3fv(BLACK);                                  // black side panels
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.30f, 0.90f, 0.f); drawBox(0.07f, 0.50f, 0.42f); glPopMatrix();
    }
    glColor3fv(BLACK);                                  // centre seam
    glPushMatrix(); glTranslatef(0.f, 0.92f, 0.215f); drawBox(0.04f, 0.52f, 0.02f); glPopMatrix();
}

// ── Belt (tan utility belt with pouches) ───────────────────────────────────────
void Deadpool::drawBelt() {
    glColor3fv(TAN);
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.f); drawBox(0.70f, 0.13f, 0.46f); glPopMatrix();
    glColor3f(0.72f, 0.60f, 0.34f);                     // buckle
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.235f); drawBox(0.12f, 0.10f, 0.02f); glPopMatrix();
    glColor3f(0.50f, 0.40f, 0.22f);                     // pouches
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.30f, 0.60f, 0.18f); drawBox(0.13f, 0.16f, 0.10f); glPopMatrix();
    }
}

// ── Katana (drawn in sword-hand-local space; handle sits inside the fist) ──────
void Deadpool::drawKatana() {
    glPushMatrix();
    glRotatef(-120.f, 1.f, 0.f, 0.f);                   // grip: blade out of the fist

    glColor3fv(BLACK);                                  // handle (in the fist)
    glPushMatrix(); drawBox(0.05f, 0.22f, 0.05f); glPopMatrix();
    glColor3f(0.22f, 0.22f, 0.24f);                     // guard
    glPushMatrix(); glTranslatef(0.f, 0.14f, 0.f); drawBox(0.16f, 0.04f, 0.06f); glPopMatrix();

    pushMetal();                                        // blade + tip (specular metal)
    glColor3f(0.82f, 0.84f, 0.90f);
    glPushMatrix();
    glTranslatef(0.f, 0.62f, 0.f); drawBox(0.045f, 0.92f, 0.02f);
    glTranslatef(0.f, 0.53f, 0.f); drawWedgePrism(0.045f, 0.14f, 0.02f);
    glPopMatrix();
    popMetal();

    glPopMatrix();
}

// ── Arms (left bare, right grips katana; spin spreads both out) ────────────────
void Deadpool::drawArms() {
    for (int s = -1; s <= 1; s += 2) {
        bool  right = (s > 0);
        float pitch = right ? kataSwing : 12.f;         // left arm slightly forward

        glPushMatrix();
        glTranslatef(s*0.36f, 1.10f, 0.f);              // shoulder pivot
        glRotatef(s*armSpread, 0.f, 0.f, 1.f);          // Skill 2: arms out
        glRotatef(-pitch,      1.f, 0.f, 0.f);          // Skill 1: sword swing

        glColor3fv(RED);                                // red upper arm
        glTranslatef(0.f, -0.20f, 0.f); drawBox(0.18f, 0.34f, 0.22f);
        glColor3fv(BLACK);                              // black forearm
        glTranslatef(0.f, -0.30f, 0.f); drawBox(0.16f, 0.30f, 0.20f);
        glColor3fv(RED);                                // red rounded hand
        glTranslatef(0.f, -0.20f, 0.f); drawSphere(0.15f, 12, 10);

        if (right) drawKatana();                        // katana parented to the hand
        glPopMatrix();
    }
}

// ── Head (big red rounded cube, angled white teardrop eyes on black patches) ──
void Deadpool::drawHead() {
    glPushMatrix();
    glTranslatef(0.f, 1.64f, 0.f);

    glColor3fv(RED);                                    // rounded-cube head
    glPushMatrix(); glScalef(1.06f, 1.04f, 0.96f); drawSphere(0.50f, 18, 16); glPopMatrix();

    glColor3fv(BLACK);                                  // black eye patches
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(s*0.20f, 0.04f, 0.40f);
        glRotatef(s*20.f, 0.f, 0.f, 1.f);
        glScalef(1.0f, 1.5f, 0.5f);
        drawSphere(0.16f, 12, 10);
        glPopMatrix();
    }
    glColor3f(0.95f, 0.96f, 0.98f);                     // white lenses
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(s*0.20f, 0.05f, 0.45f);
        glRotatef(s*20.f, 0.f, 0.f, 1.f);
        glScalef(0.7f, 1.2f, 0.4f);
        drawSphere(0.13f, 12, 10);
        glPopMatrix();
    }
    glPopMatrix();
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Deadpool::draw() {
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle + spinAngle, 0.f, 1.f, 0.f);  // spinAngle drives Skill 2

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
    // ── Skill 1: Katana Slash (forward chop) ──────────────────────────────────
    case SKILL1_WINDUP: {
        float t = phaseTimer / WINDUP_KATA;
        kataSwing = KATA_IDLE_PITCH + (KATA_RAISE_PITCH - KATA_IDLE_PITCH) * t;   // raise/cock
        if (phaseTimer >= WINDUP_KATA) { phase = SKILL1_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL1_ACTIVE: {
        float u = phaseTimer / ACTIVE_KATA;
        kataSwing = KATA_RAISE_PITCH + (KATA_CHOP_PITCH - KATA_RAISE_PITCH) * u;  // chop forward
        if (!hitRegistered && inHitRange(other)) {
            other.applyDamage(DMG_KATANA);
            hitRegistered = true; ev.hitLanded = true; ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.55f, 0.1f, 20, 4.f);
        }
        if (phaseTimer >= ACTIVE_KATA) { phase = SKILL1_RECOVERY; phaseTimer = 0.f; }
        break;
    }
    case SKILL1_RECOVERY: {
        float r = phaseTimer / RECOV_KATA;
        kataSwing = KATA_CHOP_PITCH + (KATA_IDLE_PITCH - KATA_CHOP_PITCH) * r;    // return to ready
        if (phaseTimer >= RECOV_KATA) { phase = IDLE; phaseTimer = 0.f; kataSwing = KATA_IDLE_PITCH; }
        break;
    }

    // ── Skill 2: Spinning Katana (in place) ───────────────────────────────────
    case SKILL2_WINDUP: {
        float w = phaseTimer / WINDUP_SPIN;
        armSpread = SPIN_ARMS_OUT * w;                  // arms out, katana horizontal
        kataSwing = KATA_IDLE_PITCH * (1.f - w);
        spinAngle = 0.f;
        if (phaseTimer >= WINDUP_SPIN) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL2_ACTIVE: {
        float a = phaseTimer / ACTIVE_SPIN;
        armSpread = SPIN_ARMS_OUT;
        kataSwing = 0.f;
        spinAngle = a * 360.f * SPIN_TURNS;             // whirlwind
        if (!hitRegistered && inRange(other)) {         // omni-directional hit
            other.applyDamage(DMG_SPIN);
            hitRegistered = true; ev.hitLanded = true; ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.6f, 0.15f, 22, 4.5f);
        }
        // orange trail ringing the body
        if ((int)(phaseTimer / 0.05f) > (int)((phaseTimer - dt) / 0.05f))
            ev.addEmit(x, 1.1f, z, 1.f, 0.55f, 0.12f, 4, 2.5f);
        if (phaseTimer >= ACTIVE_SPIN) { phase = SKILL2_RECOVERY; phaseTimer = 0.f; spinAngle = 0.f; }
        break;
    }
    case SKILL2_RECOVERY: {
        float r = phaseTimer / RECOV_SPIN;
        armSpread = SPIN_ARMS_OUT * (1.f - r);
        kataSwing = KATA_IDLE_PITCH * r;
        if (phaseTimer >= RECOV_SPIN) { phase = IDLE; phaseTimer = 0.f; armSpread = 0.f; kataSwing = KATA_IDLE_PITCH; }
        break;
    }

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;
    default: break;
    }

    return ev;
}
