#include "Arena.h"
#include "Constants.h"
#include <vector>
#include <cstdlib>

// ── Texture generation ────────────────────────────────────────────────────────
GLuint Arena::makeSandTexture(int N) {
    std::vector<unsigned char> px(N * N * 3);
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            int   i    = (y * N + x) * 3;
            unsigned char base = 150 + ((x/8 + y/8) % 2) * 15;
            unsigned char n    = (unsigned char)((x * 73 + y * 151) % 24);
            px[i  ] = base + n;
            px[i+1] = (unsigned char)((base - 20) + n);
            px[i+2] = (unsigned char)(90 + n / 2);
        }
    }
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, N, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    return t;
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────
void Arena::init() {
    sandTex = makeSandTexture(128);
}

void Arena::tick(float dt) {
    if (impactFlash > 0.f) impactFlash -= dt;
    if (impactFlash < 0.f) impactFlash = 0.f;
}

// ── Environment changes (Criterion 3 — do not simplify) ───────────────────────
// t: 0 = both characters at full HP, 1 = both near death.
void Arena::setEnvironmentIntensity(float t) {
    // 1. Sun light shifts from warm white → deep orange/red
    GLfloat diff[4] = { 0.9f - t*0.30f, 0.85f - t*0.50f, 0.75f - t*0.65f, 1.f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);

    // 2. Ambient shifts from warm to murky dark
    GLfloat amb[4]  = { 0.25f - t*0.10f, 0.22f - t*0.12f, 0.18f + t*0.08f, 1.f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);

    // 3. Sky colour (stored, applied in drawSky)
    skyTop = lerpColor({0.65f, 0.65f, 0.70f}, {0.35f, 0.10f, 0.05f}, t);
    skyBot = lerpColor({0.50f, 0.50f, 0.55f}, {0.20f, 0.08f, 0.04f}, t);

    // 4. Sandstorm dust rate (consumed by Game to emit particles)
    stormRate = t * 8.f;
}

// ── Sub-draw ──────────────────────────────────────────────────────────────────
void Arena::drawSky() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    // Backdrop quad very far back at z=-48
    glBegin(GL_QUADS);
    glColor3f(skyBot.r, skyBot.g, skyBot.b); glVertex3f(-60.f, -2.f, -48.f);
    glColor3f(skyBot.r, skyBot.g, skyBot.b); glVertex3f( 60.f, -2.f, -48.f);
    glColor3f(skyTop.r, skyTop.g, skyTop.b); glVertex3f( 60.f, 40.f, -48.f);
    glColor3f(skyTop.r, skyTop.g, skyTop.b); glVertex3f(-60.f, 40.f, -48.f);
    glEnd();
    glEnable(GL_LIGHTING);
}

void Arena::drawGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sandTex);
    glColor3f(1.f, 1.f, 1.f);  // white so texture colour shows through
    drawGroundPlane(ARENA_HALF + 4.f, 12);
    glDisable(GL_TEXTURE_2D);
}

void Arena::drawRuins() {
    // Two tilted tower remnants in the background (deterministic layout, srand not needed)
    // Tower 1: left-back
    glColor3f(0.45f, 0.42f, 0.38f);
    glPushMatrix();
    glTranslatef(-8.f, 0.f, -10.f);
    glRotatef(8.f, 0.f, 0.f, 1.f);   // lean slightly
    drawBox(1.2f, 6.f, 1.2f);         // main shaft
    glTranslatef(0.f, 3.5f, 0.f);
    glRotatef(15.f, 1.f, 0.f, 0.f);   // broken section
    drawBox(1.0f, 3.5f, 1.0f);
    glPopMatrix();

    // Tower 2: right-back, different lean
    glPushMatrix();
    glTranslatef(9.f, 0.f, -12.f);
    glRotatef(-12.f, 0.f, 0.f, 1.f);
    drawBox(1.4f, 8.f, 1.4f);
    glPopMatrix();

    // Broken signpost
    glColor3f(0.52f, 0.48f, 0.42f);
    glPushMatrix();
    glTranslatef(5.f, 0.f, -7.f);
    drawBox(0.18f, 3.5f, 0.18f);       // pole
    glTranslatef(0.3f, 1.8f, 0.f);
    glRotatef(25.f, 0.f, 0.f, 1.f);    // tilted sign board
    drawBox(1.4f, 0.55f, 0.12f);
    glPopMatrix();

    // Scattered rubble (small boxes, positions chosen to look random but are fixed)
    glColor3f(0.48f, 0.44f, 0.38f);
    const float rb[][3] = { {-5,0,-4},{3,0,-3},{-2,0,-6},{6,0,-5},{-7,0,-6},{1,0,-8} };
    const float rs[]    = {  0.4f, 0.5f, 0.3f, 0.6f,   0.35f,  0.45f };
    for (int i = 0; i < 6; ++i) {
        glPushMatrix();
        glTranslatef(rb[i][0], rs[i]*0.5f, rb[i][2]);
        drawBox(rs[i], rs[i], rs[i]);
        glPopMatrix();
    }
}

void Arena::draw() {
    drawSky();
    drawGround();
    drawRuins();
}
