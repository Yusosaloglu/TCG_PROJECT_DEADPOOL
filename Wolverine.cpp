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

// Palette
static const float YEL[3]  = {0.92f, 0.76f, 0.10f};  // X-suit yellow
static const float NAVY[3] = {0.12f, 0.16f, 0.46f};  // X-suit blue
static const float SKIN[3] = {0.95f, 0.79f, 0.66f};
static const float HAIR[3] = {0.06f, 0.06f, 0.08f};

// ── Constructor ───────────────────────────────────────────────────────────────
Wolverine::Wolverine() {
    x           = -2.f;
    z           =  0.f;
    facingAngle = -90.f;  // face toward Deadpool's side
}

// ── Legs ──────────────────────────────────────────────────────────────────────
void Wolverine::drawLegs() {
    glColor3fv(YEL);                                    // yellow thighs
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.42f, 0.f); drawBox(0.30f, 0.50f, 0.32f); glPopMatrix();
    }
    glColor3fv(NAVY);                                   // navy boots + rounded toes
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.13f, 0.02f); drawBox(0.34f, 0.26f, 0.40f); glPopMatrix();
        glPushMatrix(); glTranslatef(s*0.17f, 0.10f, 0.24f); glScalef(1.f, 0.8f, 1.f); drawSphere(0.14f, 12, 10); glPopMatrix();
    }
}

// ── Torso (navy body, yellow shoulders + X-suspenders, tan belt) ───────────────
void Wolverine::drawTorso() {
    glColor3fv(NAVY);
    glPushMatrix(); glTranslatef(0.f, 0.62f, 0.f); drawTrapezoidBody(0.54f, 0.66f, 0.56f, 0.42f); glPopMatrix();

    glColor3fv(YEL);                                    // yellow shoulder caps (deltoids)
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.34f, 1.12f, 0.f); drawSphere(0.20f, 14, 12); glPopMatrix();
    }

    glColor3fv(YEL);                                    // yellow X-suspenders on chest
    glPushMatrix(); glTranslatef(0.f, 0.92f, 0.205f); glRotatef( 28.f, 0,0,1); drawBox(0.07f, 0.52f, 0.03f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.f, 0.92f, 0.205f); glRotatef(-28.f, 0,0,1); drawBox(0.07f, 0.52f, 0.03f); glPopMatrix();

    glColor3f(0.45f, 0.30f, 0.12f);                     // tan belt
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.f); drawBox(0.70f, 0.13f, 0.46f); glPopMatrix();
    glColor3f(0.85f, 0.70f, 0.15f);                     // gold buckle
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.235f); drawBox(0.14f, 0.10f, 0.02f); glPopMatrix();
}

// ── Claws (drawn in glove-local space; 3 blades projecting forward +Z) ─────────
void Wolverine::drawClaws() {
    if (clawExtend < 0.02f) return;                     // hidden inside the fist when idle
    float len = clawExtend * CLAW_LEN;
    pushMetal();
    glColor3f(0.85f, 0.87f, 0.92f);
    for (int c = -1; c <= 1; ++c) {
        glPushMatrix();
        glTranslatef(c*0.07f, 0.04f, 0.12f);
        glRotatef(90.f, 1.f, 0.f, 0.f);                 // turn wedge tip to point +Z
        drawWedgePrism(0.045f, len, 0.045f);
        glPopMatrix();
    }
    popMetal();
}

// ── One arm: shoulder pivot → spin spread → slash pitch → upper/forearm/glove ──
void Wolverine::drawArm(int side) {
    glPushMatrix();
    glTranslatef(side*0.36f, 1.10f, 0.f);               // shoulder pivot
    glRotatef(side*armSpread, 0.f, 0.f, 1.f);           // Skill 2: arms out to sides
    glRotatef(-armPitch,      1.f, 0.f, 0.f);           // Skill 1: down→up sweep

    glColor3fv(YEL);                                    // yellow upper arm
    glTranslatef(0.f, -0.20f, 0.f); drawBox(0.18f, 0.34f, 0.22f);
    glColor3fv(NAVY);                                   // navy forearm
    glTranslatef(0.f, -0.30f, 0.f); drawBox(0.16f, 0.30f, 0.20f);
    glColor3fv(NAVY);                                   // navy rounded glove
    glTranslatef(0.f, -0.22f, 0.f); drawSphere(0.16f, 12, 10);

    drawClaws();                                        // claws follow the hand
    glPopMatrix();
}

// ── Head (big peach face + black swept hair, peaks, sideburns, brows, eyes) ────
void Wolverine::drawHead() {
    glPushMatrix();
    glTranslatef(0.f, 1.62f, 0.f);

    glColor3fv(HAIR);                                   // hair volume (top/back/sides)
    glPushMatrix(); glTranslatef(0.f, 0.12f, -0.10f); drawSphere(0.52f, 18, 16); glPopMatrix();

    glColor3fv(SKIN);                                   // peach face (pokes out front)
    glPushMatrix(); glTranslatef(0.f, -0.02f, 0.05f); drawSphere(0.47f, 18, 16); glPopMatrix();

    glColor3fv(HAIR);                                   // two pointed hair peaks
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(s*0.24f, 0.34f, -0.08f);
        glRotatef(s*22.f, 0.f, 0.f, 1.f);
        glRotatef(-18.f,   1.f, 0.f, 0.f);
        drawWedgePrism(0.20f, 0.34f, 0.18f);
        glPopMatrix();
    }

    glColor3fv(HAIR);                                   // mutton-chop sideburns
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.40f, -0.10f, 0.14f); glScalef(0.5f, 1.1f, 0.7f); drawSphere(0.18f, 10, 8); glPopMatrix();
    }

    glColor3f(0.05f, 0.05f, 0.06f);                     // angled eyebrows
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.08f, 0.40f); glRotatef(s*-12.f, 0,0,1); drawBox(0.16f, 0.045f, 0.03f); glPopMatrix();
    }
    glColor3f(0.10f, 0.08f, 0.07f);                     // eye dots
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.16f, -0.02f, 0.42f); drawSphere(0.045f, 8, 6); glPopMatrix();
    }
    glColor3f(0.92f, 0.74f, 0.60f);                     // small nose
    glPushMatrix(); glTranslatef(0.f, -0.12f, 0.43f); glRotatef(90.f, 1,0,0); drawWedgePrism(0.06f, 0.09f, 0.06f); glPopMatrix();

    glPopMatrix();
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Wolverine::draw() {
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle + spinAngle, 0.f, 1.f, 0.f);  // spinAngle drives Skill 2

    drawLegs();
    drawTorso();
    drawArm(-1);
    drawArm( 1);
    drawHead();

    glPopMatrix();
}

// ── Tick ──────────────────────────────────────────────────────────────────────
CharEvent Wolverine::tick(float dt, Character& other) {
    CharEvent ev;
    if (!alive) return ev;
    phaseTimer += dt;

    switch (phase) {
    // ── Skill 1: Claw Slash (down → up) ───────────────────────────────────────
    case SKILL1_WINDUP: {
        float t = phaseTimer / WINDUP_SLASH;
        clawExtend = t;                                 // claws pop out
        armPitch   = CLAW_SLASH_LOW * t;                // arms cock low
        if (phaseTimer >= WINDUP_SLASH) { phase = SKILL1_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL1_ACTIVE: {
        float u = phaseTimer / ACTIVE_SLASH;
        clawExtend = 1.f;
        armPitch   = CLAW_SLASH_LOW + (CLAW_SLASH_HIGH - CLAW_SLASH_LOW) * u;  // sweep up
        if (!hitRegistered && inHitRange(other)) {
            other.applyDamage(DMG_SLASH);
            hitRegistered = true; ev.hitLanded = true; ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.95f, 0.7f, 20, 4.f);
        }
        if (phaseTimer >= ACTIVE_SLASH) { phase = SKILL1_RECOVERY; phaseTimer = 0.f; }
        break;
    }
    case SKILL1_RECOVERY: {
        float r = phaseTimer / RECOV_SLASH;
        armPitch   = CLAW_SLASH_HIGH * (1.f - r);
        clawExtend = 1.f - r;
        if (phaseTimer >= RECOV_SLASH) { phase = IDLE; phaseTimer = 0.f; armPitch = 0.f; clawExtend = 0.f; }
        break;
    }

    // ── Skill 2: Spinning Claws (in place) ────────────────────────────────────
    case SKILL2_WINDUP: {
        float w = phaseTimer / WINDUP_SPIN;
        clawExtend = w;
        armSpread  = SPIN_ARMS_OUT * w;                 // arms out to the sides
        spinAngle  = 0.f;
        if (phaseTimer >= WINDUP_SPIN) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL2_ACTIVE: {
        float a = phaseTimer / ACTIVE_SPIN;
        clawExtend = 1.f;
        armSpread  = SPIN_ARMS_OUT;
        spinAngle  = a * 360.f * SPIN_TURNS;             // whirlwind
        if (!hitRegistered && inRange(other)) {         // omni-directional hit
            other.applyDamage(DMG_SPIN);
            hitRegistered = true; ev.hitLanded = true; ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 0.9f, 0.92f, 0.98f, 22, 4.5f);
        }
        // silver trail ringing the body
        if ((int)(phaseTimer / 0.05f) > (int)((phaseTimer - dt) / 0.05f))
            ev.addEmit(x, 1.1f, z, 0.85f, 0.88f, 0.95f, 4, 2.5f);
        if (phaseTimer >= ACTIVE_SPIN) { phase = SKILL2_RECOVERY; phaseTimer = 0.f; spinAngle = 0.f; }
        break;
    }
    case SKILL2_RECOVERY: {
        float r = phaseTimer / RECOV_SPIN;
        armSpread  = SPIN_ARMS_OUT * (1.f - r);
        clawExtend = 1.f - r;
        if (phaseTimer >= RECOV_SPIN) { phase = IDLE; phaseTimer = 0.f; armSpread = 0.f; clawExtend = 0.f; }
        break;
    }

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;
    default: break;
    }

    return ev;
}
