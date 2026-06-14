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
    facingAngle =  90.f;  // face +X, toward Deadpool
}

// Berserker Heal only triggers when the gauge holds banked healing.
void Wolverine::startSkill2() {
    if (healGauge <= 0.f) return;
    Character::startSkill2();
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

    // Mutton chops — thick pointed sideburns down the cheeks; the chin stays bare
    // skin.  Reference: the chops connect up to the hair and taper to a point at
    // the jaw — NOT a full beard.  (The old code joined them with a wide chin band
    // that buried the lower face in black, which is why he looked so heavily bearded.)
    glColor3fv(HAIR);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(s*0.33f, -0.04f, 0.19f);           // sit on the side of the cheek
        glRotatef(s*14.f, 0.f, 0.f, 1.f);               // flare to follow the jawline
        glPushMatrix(); glScalef(0.55f, 1.35f, 0.90f); drawSphere(0.20f, 12, 10); glPopMatrix();  // chop body
        glPushMatrix();                                 // pointed tip tapering to the jaw
        glTranslatef(0.f, -0.26f, 0.02f);
        glRotatef(180.f, 1.f, 0.f, 0.f);                // wedge tip points down
        drawWedgePrism(0.15f, 0.18f, 0.16f);
        glPopMatrix();
        glPopMatrix();
    }

    // Eyes — large round black eyes, sitting proud of the face surface
    glColor3f(0.06f, 0.05f, 0.05f);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.165f, 0.01f, 0.46f); glScalef(1.0f, 1.15f, 0.55f); drawSphere(0.085f, 14, 12); glPopMatrix();
    }
    // Thick angled eyebrows — the scowl
    glColor3f(0.05f, 0.05f, 0.06f);
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.17f, 0.15f, 0.46f); glRotatef(s*-18.f, 0,0,1); drawBox(0.20f, 0.065f, 0.05f); glPopMatrix();
    }
    // Nose
    glColor3f(0.93f, 0.76f, 0.62f);
    glPushMatrix(); glTranslatef(0.f, -0.05f, 0.50f); glRotatef(90.f, 1,0,0); drawWedgePrism(0.08f, 0.12f, 0.08f); glPopMatrix();

    glPopMatrix();
}

// ── Heal aura (translucent green additive glow during Berserker Heal) ──────────
void Wolverine::drawHealAura() {
    float intensity;
    if      (phase == SKILL2_WINDUP)   intensity = phaseTimer / WINDUP_HEAL;
    else if (phase == SKILL2_ACTIVE)   intensity = 1.f;
    else if (phase == SKILL2_RECOVERY) intensity = 1.f - phaseTimer / RECOV_HEAL;
    else return;
    intensity = clampf(intensity, 0.f, 1.f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);              // additive glow
    glDepthMask(GL_FALSE);

    glColor4f(0.25f, 1.0f, 0.45f, HEAL_AURA_ALPHA * intensity);
    glPushMatrix();
    glTranslatef(0.f, 1.0f, 0.f);                   // centre on the torso
    drawSphere(HEAL_AURA_RADIUS, 16, 12);
    glPopMatrix();

    glDepthMask(GL_TRUE);                           // restore default GL state
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Wolverine::draw() {
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle, 0.f, 1.f, 0.f);

    if (rollTimer > 0.f) {                       // dodge-roll tumble about the forward axis
        float t    = 1.f - rollTimer / ROLL_DUR;
        float lift = sinf((t > 1.f ? 1.f : t) * 3.14159265f) * ROLL_HOP;
        glTranslatef(0.f, ROLL_PIVOT_Y + lift, 0.f);
        glRotatef(rollAngle, 0.f, 0.f, 1.f);
        glTranslatef(0.f, -ROLL_PIVOT_Y, 0.f);
    }

    drawLegs();
    drawTorso();
    drawArm(-1);
    drawArm( 1);
    drawHead();
    drawHealAura();          // green glow during Berserker Heal

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
        if (!hitRegistered && inHitRange(other) && !other.rolling()) {  // roll dodges (i-frames)
            other.applyDamage(DMG_SLASH);
            hitRegistered = true; ev.hitLanded = true; ev.doImpactFlash = true;
            ev.addEmit(other.x, 1.2f, other.z, 1.f, 0.95f, 0.7f, 20, 4.f);
            healGauge = clampf(healGauge + HEAL_GAIN_PER_HIT, 0.f, HEAL_GAUGE_MAX);  // bank healing
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

    // ── Skill 2: Berserker Heal (gauge fuelled by landed Claw Slashes) ─────────
    case SKILL2_WINDUP: {
        // aura ramps in (drawn by drawHealAura); no HP change yet
        if (phaseTimer >= WINDUP_HEAL) {
            phase = SKILL2_ACTIVE; phaseTimer = 0.f;
            healBanked = healGauge;                     // snapshot what we pour back
        }
        break;
    }
    case SKILL2_ACTIVE: {
        hp = clampf(hp + healBanked * (dt / ACTIVE_HEAL), 0.f, MAX_HP);  // HP bar visibly climbs
        if ((int)(phaseTimer / 0.06f) > (int)((phaseTimer - dt) / 0.06f))
            ev.addEmit(x, 1.0f, z, 0.3f, 1.f, 0.5f, 4, 2.0f);            // rising green motes
        if (phaseTimer >= ACTIVE_HEAL) {
            phase = SKILL2_RECOVERY; phaseTimer = 0.f;
            healGauge = 0.f;                            // gauge spent
        }
        break;
    }
    case SKILL2_RECOVERY: {
        // aura ramps out
        if (phaseTimer >= RECOV_HEAL) { phase = IDLE; phaseTimer = 0.f; }
        break;
    }

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;
    default: break;
    }

    return ev;
}
