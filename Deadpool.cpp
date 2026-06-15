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

// ── Desert Eagle (hand-built hand-cannon) ──────────────────────────────────────
// After the 180° flip the local frame is: +Y = forward (aim), +Z = down,
// +X = sideways.  Built as an L-shape — long slide/barrel forward, grip hanging
// down — so it always reads as a pistol (the old straight rod looked like a knife).
void Deadpool::drawDesertEagle() {
    const float GM[3] = {0.40f, 0.41f, 0.44f};   // gunmetal steel
    const float BK[3] = {0.09f, 0.09f, 0.10f};   // black grip / furniture

    glPushMatrix();
    glRotatef(180.f, 1.f, 0.f, 0.f);             // +Y → forward (aim), +Z → down

    // ── Grip: rakes down (+Z) and back (-Y), wrapped by the fist ──
    glPushMatrix();
        glTranslatef(0.f, -0.06f, 0.15f);
        glRotatef(14.f, 1.f, 0.f, 0.f);                                  // rake backward
        glColor3fv(BK);
        drawBox(0.11f, 0.13f, 0.30f);                                    // long along +Z (down)
        glColor3f(0.17f, 0.17f, 0.18f);                                  // side grip panels
        glPushMatrix(); glTranslatef( 0.058f, 0.f, 0.02f); drawBox(0.013f, 0.10f, 0.22f); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.058f, 0.f, 0.02f); drawBox(0.013f, 0.10f, 0.22f); glPopMatrix();
    glPopMatrix();

    // ── Frame / receiver: chunky block linking grip to slide ──
    glColor3fv(GM);
    glPushMatrix(); glTranslatef(0.f, 0.05f, 0.005f); drawBox(0.12f, 0.30f, 0.14f); glPopMatrix();

    // ── Trigger guard (squared loop) + trigger ──
    glColor3fv(BK);
    glPushMatrix(); glTranslatef(0.f, 0.10f, 0.15f);  drawBox(0.05f, 0.14f, 0.02f); glPopMatrix();  // bottom (runs fwd)
    glPushMatrix(); glTranslatef(0.f, 0.17f, 0.09f);  drawBox(0.05f, 0.02f, 0.13f); glPopMatrix();  // front (runs down)
    glColor3f(0.20f, 0.20f, 0.22f);
    glPushMatrix(); glTranslatef(0.f, 0.07f, 0.085f); drawBox(0.03f, 0.05f, 0.06f); glPopMatrix();  // trigger

    // ── Slide: the big Desert Eagle top, runs forward (+Y), sits high (-Z) ──
    pushMetal();
    glColor3fv(GM);
    glPushMatrix(); glTranslatef(0.f, 0.21f, -0.07f); drawBox(0.13f, 0.46f, 0.15f); glPopMatrix();

    // ── Barrel rib on top (DE signature) + front & rear sights ──
    glColor3f(0.32f, 0.33f, 0.36f);
    glPushMatrix(); glTranslatef(0.f, 0.21f, -0.16f); drawBox(0.06f, 0.46f, 0.04f); glPopMatrix();  // full-length rib
    glColor3fv(BK);
    glPushMatrix(); glTranslatef(0.f, 0.40f, -0.19f); drawBox(0.05f, 0.04f, 0.05f); glPopMatrix();  // front sight
    glPushMatrix(); glTranslatef(0.f, 0.01f, -0.19f); drawBox(0.06f, 0.04f, 0.05f); glPopMatrix();  // rear sight
    popMetal();

    // ── Muzzle: squared crown with a dark bore at the very front ──
    glColor3f(0.24f, 0.24f, 0.26f);
    glPushMatrix(); glTranslatef(0.f, 0.45f, -0.06f); drawBox(0.11f, 0.06f, 0.15f); glPopMatrix();
    glColor3f(0.02f, 0.02f, 0.02f);
    glPushMatrix(); glTranslatef(0.f, 0.49f, -0.06f); drawBox(0.045f, 0.02f, 0.06f); glPopMatrix();  // bore hole

    // ── Hammer at the rear (exposed, as on a real DE) ──
    glColor3fv(BK);
    glPushMatrix(); glTranslatef(0.f, -0.05f, -0.08f); glRotatef(-22.f, 1,0,0); drawBox(0.05f, 0.05f, 0.09f); glPopMatrix();

    glPopMatrix();
}

// ── Desert Eagle tracer: additive beam from muzzle to impact, in world space ───
void Deadpool::drawTracer() {
    if (tracerTimer <= 0.f) return;
    float a = tracerTimer / TRACER_DUR;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);          // additive
    glDepthMask(GL_FALSE);

    glLineWidth(3.f);
    glColor4f(1.f, 0.9f, 0.5f, a);
    glBegin(GL_LINES);
    glVertex3f(tracerSx, tracerSy, tracerSz);
    glVertex3f(tracerEx, tracerEy, tracerEz);
    glEnd();
    glLineWidth(1.f);

    glDepthMask(GL_TRUE);                        // restore default GL state
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ── Arms (right hand wields katana or Desert Eagle; left mirrors for the sword) ─
void Deadpool::drawArms() {
    bool gunning  = (phase == SKILL2_WINDUP || phase == SKILL2_ACTIVE || phase == SKILL2_RECOVERY);
    bool slashing = (phase == SKILL1_WINDUP || phase == SKILL1_ACTIVE || phase == SKILL1_RECOVERY);

    for (int s = -1; s <= 1; s += 2) {
        bool right = (s > 0);

        // Pose + weapon per phase
        float pitch, yaw = 0.f;
        bool  extend = false, holdKatana = false, holdGun = false;
        if (gunning && right) {               // gun arm punches straight forward
            pitch = GUN_AIM_PITCH;
            if (phase == SKILL2_ACTIVE) pitch -= GUN_RECOIL_DEG;   // recoil kick on fire
            holdGun = true;
        } else if (gunning) {                 // off hand tucked low while shooting
            pitch = KATA_IDLE_PITCH * 0.4f;
        } else if (slashing && right) {       // sword arm horizontal, sweeping L→R
            pitch = KATA_SLASH_PITCH; yaw = kataYaw; extend = true; holdKatana = true;
        } else {                              // idle: hand forward, blade standing up
            pitch = KATA_IDLE_PITCH;  holdKatana = true;
        }

        glPushMatrix();
        glTranslatef(s*0.36f, 1.10f, 0.f);          // shoulder pivot
        glRotatef(yaw,      0.f, 1.f, 0.f);         // Skill 1: horizontal sweep
        glRotatef(-pitch,   1.f, 0.f, 0.f);         // arm pitch

        glColor3fv(RED);                            // red upper arm
        glTranslatef(0.f, -0.20f, 0.f); drawBox(0.18f, 0.34f, 0.22f);
        glColor3fv(BLACK);                          // black forearm
        glTranslatef(0.f, -0.30f, 0.f); drawBox(0.16f, 0.30f, 0.20f);
        glColor3fv(RED);                            // red rounded hand
        glTranslatef(0.f, -0.20f, 0.f); drawSphere(0.15f, 12, 10);

        if (holdKatana) drawKatana(pitch, extend);
        if (holdGun)    drawDesertEagle();
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
        glScalef(0.65f, 0.60f, 0.60f);
        drawSphere(0.12f, 12, 10);
        glPopMatrix();
    }
    glPopMatrix();
}

// ── Main draw ─────────────────────────────────────────────────────────────────
void Deadpool::draw() {
    glPushMatrix();
    glTranslatef(x, 0.f, z);
    glRotatef(facingAngle, 0.f, 1.f, 0.f);

    drawLegs();
    drawTorso();
    drawBackGear();
    drawBelt();
    drawArms();
    drawHead();

    glPopMatrix();

    drawTracer();   // world-space shot beam (after the body matrix is popped)
}

// ── Tick ──────────────────────────────────────────────────────────────────────
CharEvent Deadpool::tick(float dt, Character& other) {
    CharEvent ev;
    if (!alive) return ev;
    phaseTimer += dt;
    if (tracerTimer > 0.f) tracerTimer -= dt;   // fade the shot beam

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

    // ── Skill 2: Desert Eagle (instant hitscan + tracer) ──────────────────────
    case SKILL2_WINDUP: {
        // raise the gun into the aim pose (handled by drawArms)
        if (phaseTimer >= WINDUP_GUN) { phase = SKILL2_ACTIVE; phaseTimer = 0.f; }
        break;
    }
    case SKILL2_ACTIVE: {
        if (!hitRegistered) {                           // fire exactly once per cast
            hitRegistered = true;
            float rad = facingAngle * (float)M_PI / 180.f;
            float fx = sinf(rad), fz = cosf(rad);       // forward
            float rx = cosf(rad), rz = -sinf(rad);      // right (matches strafe basis)
            tracerSx = x + fx*MUZZLE_FWD + rx*MUZZLE_SIDE;
            tracerSy = MUZZLE_HEIGHT;
            tracerSz = z + fz*MUZZLE_FWD + rz*MUZZLE_SIDE;
            tracerEx = tracerSx + fx*GUN_RANGE;
            tracerEy = MUZZLE_HEIGHT;
            tracerEz = tracerSz + fz*GUN_RANGE;
            tracerTimer = TRACER_DUR;

            ev.addEmit(tracerSx, tracerSy, tracerSz, 1.f, 0.85f, 0.3f, 14, 5.f);  // muzzle flash

            if (inCone(other, GUN_RANGE, GUN_ANGLE_TOL)) {                         // hitscan
                other.applyDamage(DMG_GUN);
                ev.hitLanded = true; ev.doImpactFlash = true;
                ev.addEmit(other.x, 1.2f, other.z, 0.7f, 0.05f, 0.05f, 18, 4.f);  // blood
            }
        }
        if (phaseTimer >= ACTIVE_GUN) { phase = SKILL2_RECOVERY; phaseTimer = 0.f; }
        break;
    }
    case SKILL2_RECOVERY: {
        // lower the gun (handled by drawArms)
        if (phaseTimer >= RECOV_GUN) { phase = IDLE; phaseTimer = 0.f; }
        break;
    }

    case HIT_STUN:
        if (phaseTimer >= 0.3f) { phase = IDLE; phaseTimer = 0.f; }
        break;
    default: break;
    }

    return ev;
}
