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
    facingAngle = -90.f;  // face -X, toward Wolverine
    kataYaw     =  0.f;
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

    // round Deadpool logo buckle: black ring, red centre, crossed katanas
    glColor3fv(BLACK);
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.243f); glScalef(1.f, 1.f, 0.35f); drawSphere(0.095f, 16, 12); glPopMatrix();
    glColor3fv(RED);
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.250f); glScalef(1.f, 1.f, 0.35f); drawSphere(0.062f, 14, 10); glPopMatrix();
    glColor3f(0.82f, 0.84f, 0.90f);
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.262f); glRotatef( 35.f, 0,0,1); drawBox(0.018f, 0.15f, 0.01f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.f, 0.64f, 0.262f); glRotatef(-35.f, 0,0,1); drawBox(0.018f, 0.15f, 0.01f); glPopMatrix();

    glColor3f(0.50f, 0.40f, 0.22f);                     // pouches
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.30f, 0.60f, 0.18f); drawBox(0.13f, 0.16f, 0.10f); glPopMatrix();
    }
}

// ── Back gear: X-harness straps + twin scabbards (trademark katana holders) ────
void Deadpool::drawBackGear() {
    glColor3fv(BLACK);                                  // X-harness straps on the back
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(0.f, 0.95f, -0.205f); glRotatef(s*32.f, 0,0,1); drawBox(0.06f, 0.62f, 0.02f); glPopMatrix();
    }
    glColor3f(0.15f, 0.15f, 0.16f);                     // twin scabbards, tips above shoulders
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix();
        glTranslatef(0.f, 1.18f, -0.30f);
        glRotatef(s*24.f, 0.f, 0.f, 1.f);
        drawBox(0.075f, 1.05f, 0.075f);
        glPopMatrix();
    }
    glColor3fv(TAN);                                    // sheath collars at the openings
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s*0.30f, 1.55f, -0.34f); drawBox(0.10f, 0.07f, 0.10f); glPopMatrix();
    }
}

// ── Katana (drawn in hand-local space) ────────────────────────────────────────
// extend=false: blade stands up (idle), uprightDeg cancels the arm pitch.
// extend=true : blade points out along the arm (forward for the slash, outward
//               for the spin) — never up through the face.
void Deadpool::drawKatana(float uprightDeg, bool extend) {
    glPushMatrix();
    if (extend) glRotatef(180.f, 1.f, 0.f, 0.f);        // build blade down the arm (-Y local)
    else        glRotatef(uprightDeg, 1.f, 0.f, 0.f);   // stand blade vertical

    glColor3fv(BLACK);                                  // handle (in the fist)
    drawBox(0.05f, 0.24f, 0.05f);
    glColor3f(0.22f, 0.22f, 0.24f);                     // guard
    glPushMatrix(); glTranslatef(0.f, 0.16f, 0.f); drawBox(0.16f, 0.045f, 0.06f); glPopMatrix();

    pushMetal();                                        // blade + tip (specular metal)
    glColor3f(0.82f, 0.84f, 0.90f);
    glPushMatrix();
    glTranslatef(0.f, 0.68f, 0.f); drawBox(0.045f, 0.98f, 0.02f);
    glTranslatef(0.f, 0.56f, 0.f); drawWedgePrism(0.05f, 0.16f, 0.02f);
    glPopMatrix();
    popMetal();

    glPopMatrix();
}

// ── Arms (both hands grip a katana; right arm slashes L→R, both spin out) ──────
void Deadpool::drawArms() {
    bool spinning = (phase == SKILL2_WINDUP || phase == SKILL2_ACTIVE || phase == SKILL2_RECOVERY);
    bool slashing = (phase == SKILL1_WINDUP || phase == SKILL1_ACTIVE || phase == SKILL1_RECOVERY);

    for (int s = -1; s <= 1; s += 2) {
        bool right = (s > 0);

        // Pose per phase
        float spread = armSpread;     // nonzero only during spin
        float pitch, yaw;
        bool  extend;
        if (spinning) {               // arms straight out to the sides, blades outward
            pitch = 0.f;  yaw = 0.f;  extend = true;
        } else if (slashing && right) {   // sword arm horizontal, sweeping L→R
            pitch = KATA_SLASH_PITCH; yaw = kataYaw; extend = true;
        } else {                      // idle: hand forward, blade standing up
            pitch = KATA_IDLE_PITCH;  yaw = 0.f;     extend = false;
        }

        glPushMatrix();
        glTranslatef(s*0.36f, 1.10f, 0.f);          // shoulder pivot
        glRotatef(s*spread, 0.f, 0.f, 1.f);         // Skill 2: arms out to sides
        glRotatef(yaw,      0.f, 1.f, 0.f);         // Skill 1: horizontal sweep
        glRotatef(-pitch,   1.f, 0.f, 0.f);         // arm pitch

        glColor3fv(RED);                            // red upper arm
        glTranslatef(0.f, -0.20f, 0.f); drawBox(0.18f, 0.34f, 0.22f);
        glColor3fv(BLACK);                          // black forearm
        glTranslatef(0.f, -0.30f, 0.f); drawBox(0.16f, 0.30f, 0.20f);
        glColor3fv(RED);                            // red rounded hand
        glTranslatef(0.f, -0.20f, 0.f); drawSphere(0.15f, 12, 10);

        drawKatana(pitch, extend);                  // both hands hold a katana
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
    drawBackGear();
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
    // ── Skill 1: Katana Slash (horizontal, left → right) ──────────────────────
    case SKILL1_WINDUP: {
        float t = phaseTimer / WINDUP_KATA;
        kataYaw = KATA_SLASH_YAW * t;                   // cock to the left
        if (phaseTimer >= WINDUP_KATA) { phase = SKILL1_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL1_ACTIVE: {
        float u = phaseTimer / ACTIVE_KATA;
        kataYaw = KATA_SLASH_YAW - 2.f * KATA_SLASH_YAW * u;   // sweep +yaw → -yaw (L→R)
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
        kataYaw = -KATA_SLASH_YAW * (1.f - r);          // return to centre
        if (phaseTimer >= RECOV_KATA) { phase = IDLE; phaseTimer = 0.f; kataYaw = 0.f; }
        break;
    }

    // ── Skill 2: Spinning Katana (in place, blades out) ───────────────────────
    case SKILL2_WINDUP: {
        float w = phaseTimer / WINDUP_SPIN;
        armSpread = SPIN_ARMS_OUT * w;                  // arms out, katanas swing outward
        spinAngle = 0.f;
        if (phaseTimer >= WINDUP_SPIN) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL2_ACTIVE: {
        float a = phaseTimer / ACTIVE_SPIN;
        armSpread = SPIN_ARMS_OUT;
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
        armSpread = SPIN_ARMS_OUT * (1.f - r);          // arms swing back to the body
        if (phaseTimer >= RECOV_SPIN) { phase = IDLE; phaseTimer = 0.f; armSpread = 0.f; }
        break;
    }

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;
    default: break;
    }

    return ev;
}
