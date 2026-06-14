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

// Procedural overcast-cloud texture (grayscale value-noise fbm).
static float cloudHash(int x, int y) {
    int n = x*374761393 + y*668265263;
    n = (n ^ (n >> 13)) * 1274126177;
    return (float)((n ^ (n >> 16)) & 0xffff) / 65535.f;
}
static float cloudNoise(float x, float y) {
    int xi = (int)floorf(x), yi = (int)floorf(y);
    float xf = x - xi, yf = y - yi;
    float a = cloudHash(xi,   yi  ), b = cloudHash(xi+1, yi  );
    float c = cloudHash(xi,   yi+1), d = cloudHash(xi+1, yi+1);
    float u = xf*xf*(3.f-2.f*xf), v = yf*yf*(3.f-2.f*yf);
    return a*(1-u)*(1-v) + b*u*(1-v) + c*(1-u)*v + d*u*v;
}
GLuint Arena::makeCloudTexture(int N) {
    std::vector<unsigned char> px(N * N * 3);
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            float fx = (float)x / N * 6.f, fy = (float)y / N * 6.f;
            float f = 0.f, amp = 0.5f, frq = 1.f;
            for (int o = 0; o < 4; ++o) { f += amp * cloudNoise(fx*frq, fy*frq); frq *= 2.f; amp *= 0.5f; }
            float g = 0.55f + f * 0.7f;                 // cloud brightness
            if (g > 1.f) g = 1.f;
            unsigned char c = (unsigned char)(g * 255.f);
            int i = (y*N + x) * 3;
            px[i] = c; px[i+1] = c; px[i+2] = c;        // grayscale; tinted by sky gradient via MODULATE
        }
    }
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, N, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    return t;
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────
void Arena::init() {
    sandTex  = makeSandTexture(128);
    cloudTex = makeCloudTexture(256);
    seedDust();
}

static float randf() { return (float)rand() / (float)RAND_MAX; }

// Place a dust grain on the upwind edge (or anywhere, for the initial seed).
void Arena::respawnDust(Dust& d, bool anywhere) {
    d.x = anywhere ? (-14.f + randf()*28.f) : -14.f;
    d.y = 0.05f + randf()*3.2f;
    d.z = -16.f + randf()*22.f;
    d.vx = (randf()-0.5f)*0.6f;
    d.vy = (randf()-0.3f)*0.3f;
    d.vz = (randf()-0.5f)*0.4f;
    d.maxLife = 3.f + randf()*4.f;
    d.life = anywhere ? randf()*d.maxLife : d.maxLife;
    d.size = 0.06f + randf()*0.10f;
}

void Arena::seedDust() {
    for (int i = 0; i < DUST_COUNT; ++i) respawnDust(dust[i], true);
}

void Arena::tick(float dt) {
    if (impactFlash > 0.f) impactFlash -= dt;
    if (impactFlash < 0.f) impactFlash = 0.f;

    // Sand wind — global gusting drift along +X, scaled up as the fight heats.
    windPhase += dt;
    float gust = WIND_BASE
               + WIND_GUST * (0.5f + 0.5f * sinf(windPhase * 0.6f))
               + envT * 2.4f;
    for (int i = 0; i < DUST_COUNT; ++i) {
        Dust& d = dust[i];
        d.x += (gust + d.vx) * dt;
        d.z += (gust * 0.22f + d.vz) * dt;
        d.y += d.vy * dt + sinf((windPhase + d.x) * 1.6f) * 0.04f * dt;
        d.life -= dt;
        if (d.life <= 0.f || d.x > 14.f) respawnDust(d, false);
    }
}

// ── Environment changes (Criterion 3 — do not simplify) ───────────────────────
// t: 0 = both characters at full HP, 1 = both near death.
void Arena::setEnvironmentIntensity(float t) {
    envT = t;

    // 1. Sun light shifts from warm white → deep orange/red
    GLfloat diff[4] = { 0.9f - t*0.30f, 0.85f - t*0.50f, 0.75f - t*0.65f, 1.f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);

    // 2. Ambient shifts from warm to murky dark
    GLfloat amb[4]  = { 0.25f - t*0.10f, 0.22f - t*0.12f, 0.18f + t*0.08f, 1.f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);

    // 3. Sky colour (stored, applied in drawSky)
    skyTop = lerpColor({0.65f, 0.65f, 0.70f}, {0.35f, 0.10f, 0.05f}, t);
    skyBot = lerpColor({0.50f, 0.50f, 0.55f}, {0.20f, 0.08f, 0.04f}, t);
}

// ── Sub-draw ──────────────────────────────────────────────────────────────────
void Arena::drawSky() {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  // clouds × sky gradient
    // Far backdrop quad; vertex colours give the vertical gradient, texture the clouds.
    glBegin(GL_QUADS);
    glColor3f(skyBot.r, skyBot.g, skyBot.b); glTexCoord2f(0.f, 0.f); glVertex3f(-85.f, -4.f, -50.f);
    glColor3f(skyBot.r, skyBot.g, skyBot.b); glTexCoord2f(1.f, 0.f); glVertex3f( 85.f, -4.f, -50.f);
    glColor3f(skyTop.r, skyTop.g, skyTop.b); glTexCoord2f(1.f, 1.f); glVertex3f( 85.f, 46.f, -50.f);
    glColor3f(skyTop.r, skyTop.g, skyTop.b); glTexCoord2f(0.f, 1.f); glVertex3f(-85.f, 46.f, -50.f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void Arena::drawGround() {
    // One large tiled sand desert covering the whole visible field.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sandTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.f, 1.f, 1.f);
    drawGroundPlane(60.f, 44);
    glDisable(GL_TEXTURE_2D);
}

// ── Sand-wind dust: alpha-blended tan streaks drifting with the wind ───────────
void Arena::drawDust() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBegin(GL_QUADS);
    for (int i = 0; i < DUST_COUNT; ++i) {
        const Dust& d = dust[i];
        float fade = sinf(3.14159f * (d.life / d.maxLife));   // fade in then out
        if (fade < 0.f) fade = 0.f;
        float a = DUST_ALPHA * fade * (0.6f + envT * 0.8f);
        glColor4f(0.86f, 0.79f, 0.62f, a);
        float w = d.size * 3.0f, h = d.size;               // stretched along the wind (X)
        glVertex3f(d.x - w, d.y - h, d.z);
        glVertex3f(d.x + w, d.y - h, d.z);
        glVertex3f(d.x + w, d.y + h, d.z);
        glVertex3f(d.x - w, d.y + h, d.z);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ── Background structure builders (draw at local origin; caller positions) ─────

// Leaning broken CN-tower spire with bulbous observation deck + antenna.
static void buildSpire() {
    glColor3f(0.55f, 0.53f, 0.50f);
    glPushMatrix();
    glTranslatef(0,2.f,0); drawBox(0.9f, 4.f, 0.9f);
    glTranslatef(0,3.f,0); drawBox(0.6f, 2.f, 0.6f);
    glPopMatrix();
    glColor3f(0.50f, 0.48f, 0.45f);                     // observation bulb
    glPushMatrix(); glTranslatef(0,5.6f,0); glScalef(1.6f,0.9f,1.6f); drawSphere(0.7f,16,12); glPopMatrix();
    glColor3f(0.44f, 0.42f, 0.40f);                     // antenna mast
    glPushMatrix(); glTranslatef(0,7.4f,0); drawBox(0.16f, 2.6f, 0.16f); glPopMatrix();
}

// Steel lattice billboard / signal tower with cross-bracing and a top panel.
static void buildLatticeTower() {
    glColor3f(0.40f, 0.38f, 0.36f);
    const float h = 6.0f, w = 1.3f;
    for (int sx=-1; sx<=1; sx+=2)                        // 4 vertical legs
        for (int sz=-1; sz<=1; sz+=2) {
            glPushMatrix(); glTranslatef(sx*w*0.5f, h*0.5f, sz*w*0.5f); drawBox(0.10f, h, 0.10f); glPopMatrix();
        }
    for (int i=0; i<=4; ++i) {                           // horizontal rings
        float y = h*i/4.f;
        glPushMatrix(); glTranslatef(0,y, w*0.5f); drawBox(w, 0.07f, 0.07f); glPopMatrix();
        glPushMatrix(); glTranslatef(0,y,-w*0.5f); drawBox(w, 0.07f, 0.07f); glPopMatrix();
        glPushMatrix(); glTranslatef( w*0.5f,y,0); drawBox(0.07f, 0.07f, w); glPopMatrix();
        glPushMatrix(); glTranslatef(-w*0.5f,y,0); drawBox(0.07f, 0.07f, w); glPopMatrix();
    }
    for (int i=0; i<4; ++i) {                            // diagonal braces (front/back)
        float y = h*i/4.f + h/8.f;
        glPushMatrix(); glTranslatef(0,y, w*0.5f); glRotatef( 35.f,0,0,1); drawBox(0.06f, w*0.95f, 0.06f); glPopMatrix();
        glPushMatrix(); glTranslatef(0,y,-w*0.5f); glRotatef(-35.f,0,0,1); drawBox(0.06f, w*0.95f, 0.06f); glPopMatrix();
    }
    glColor3f(0.30f, 0.32f, 0.36f);                     // top panel array
    glPushMatrix(); glTranslatef(0, h+0.55f, 0); drawBox(2.4f, 1.5f, 0.12f); glPopMatrix();
    glColor3f(0.18f, 0.20f, 0.24f);
    for (int i=-1; i<=1; ++i) { glPushMatrix(); glTranslatef(i*0.7f, h+0.55f, 0.08f); drawBox(0.04f, 1.5f, 0.02f); glPopMatrix(); }
}

// Toppled sandstone building — a pile of tilted cubes.
static void buildCollapsedBuilding() {
    glColor3f(0.80f, 0.72f, 0.52f);
    glPushMatrix(); glRotatef(18.f,0,0,1); glTranslatef(0,1.2f,0); drawBox(2.4f, 2.4f, 2.4f); glPopMatrix();
    glPushMatrix(); glTranslatef(1.9f,0.7f,0.4f);  glRotatef(-25.f,0,0,1); drawBox(1.6f, 1.6f, 1.8f); glPopMatrix();
    glColor3f(0.72f, 0.64f, 0.46f);
    glPushMatrix(); glTranslatef(-1.7f,0.55f,-0.6f); glRotatef(14.f,1,0,1); drawBox(1.3f, 1.3f, 1.3f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.5f,0.5f,1.7f);   glRotatef(30.f,0,1,0); drawBox(1.1f, 1.0f, 1.1f); glPopMatrix();
}

// Open market-stall frame: four legs + a flat roof.
static void buildStall() {
    const float w=1.5f, d=1.1f, h=1.0f;
    glColor3f(0.40f, 0.34f, 0.26f);
    for (int sx=-1; sx<=1; sx+=2)
        for (int sz=-1; sz<=1; sz+=2) {
            glPushMatrix(); glTranslatef(sx*w*0.5f, h*0.5f, sz*d*0.5f); drawBox(0.08f, h, 0.08f); glPopMatrix();
        }
    glColor3f(0.47f, 0.41f, 0.31f);
    glPushMatrix(); glTranslatef(0,h,0); drawBox(w+0.2f, 0.10f, d+0.2f); glPopMatrix();
}

// Tilted broken signpost.
static void buildSignpost(float tilt) {
    glPushMatrix(); glRotatef(tilt, 0,0,1);
    glColor3f(0.50f, 0.48f, 0.42f);
    glPushMatrix(); glTranslatef(0,1.4f,0); drawBox(0.12f, 2.8f, 0.12f); glPopMatrix();
    glColor3f(0.60f, 0.56f, 0.50f);
    glPushMatrix(); glTranslatef(0.45f,2.4f,0); glRotatef(14.f,0,0,1); drawBox(1.2f, 0.5f, 0.08f); glPopMatrix();
    glPopMatrix();
}

// Wrecked vehicle hull.
static void buildWreck() {
    glColor3f(0.32f, 0.30f, 0.30f);
    glPushMatrix(); glTranslatef(0,0.5f,0); drawBox(2.4f, 0.9f, 1.2f); glPopMatrix();
    glColor3f(0.28f, 0.26f, 0.26f);
    glPushMatrix(); glTranslatef(-0.2f,1.1f,0); drawBox(1.3f, 0.6f, 1.1f); glPopMatrix();
    glColor3f(0.12f, 0.12f, 0.12f);                     // wheels
    for (int sx=-1; sx<=1; sx+=2)
        for (int sz=-1; sz<=1; sz+=2) {
            glPushMatrix(); glTranslatef(sx*0.85f,0.25f,sz*0.62f); glScalef(1.f,1.f,0.5f); drawSphere(0.28f,12,8); glPopMatrix();
        }
}

// Distant low hills on the horizon.
static void buildHills() {
    glColor3f(0.52f, 0.50f, 0.48f);
    const float hx[5] = {-30,-14, 2, 18, 32};
    const float hs[5] = { 10, 14,12, 16, 11};
    for (int i=0; i<5; ++i) {
        glPushMatrix(); glTranslatef(hx[i], -1.f, -34.f); glScalef(hs[i], 4.f, 6.f); drawSphere(1.f, 14, 10); glPopMatrix();
    }
}

void Arena::drawRuins() {
    buildHills();

    // Leaning CN-tower spire (centre-back) + its fallen observation pod
    glPushMatrix(); glTranslatef(-3.f, 0.f, -18.f); glRotatef(22.f,0,0,1); buildSpire(); glPopMatrix();
    glColor3f(0.50f, 0.48f, 0.45f);
    glPushMatrix(); glTranslatef(0.8f, 0.6f, -14.5f); glScalef(1.8f,1.0f,1.4f); drawSphere(1.f,16,12); glPopMatrix();

    // Steel lattice tower (right-back)
    glPushMatrix(); glTranslatef(10.5f, 0.f, -12.f); buildLatticeTower(); glPopMatrix();

    // Collapsed sandstone building (left)
    glPushMatrix(); glTranslatef(-10.5f, 0.f, -9.f); buildCollapsedBuilding(); glPopMatrix();

    // Market stalls (mid-ground)
    glPushMatrix(); glTranslatef(3.8f,  0.f, -9.f);  buildStall(); glPopMatrix();
    glPushMatrix(); glTranslatef(-4.5f, 0.f, -10.5f); glRotatef(12.f,0,1,0); buildStall(); glPopMatrix();

    // Wrecked vehicle
    glPushMatrix(); glTranslatef(7.5f, 0.f, -11.f); glRotatef(-22.f,0,1,0); buildWreck(); glPopMatrix();

    // Signposts
    glPushMatrix(); glTranslatef( 5.5f, 0.f, -6.5f); buildSignpost( 12.f); glPopMatrix();
    glPushMatrix(); glTranslatef(-6.5f, 0.f, -7.f);  buildSignpost(-18.f); glPopMatrix();
    glPushMatrix(); glTranslatef( 2.f,  0.f, -12.f); buildSignpost(  6.f); glPopMatrix();

    // Scattered rubble around the perimeter (deterministic; behind the play area)
    glColor3f(0.50f, 0.46f, 0.40f);
    const float rb[][3] = {
        {-7,0,-4},{6.5f,0,-4.5f},{-3,0,-6},{8,0,-6},{-9,0,-3.5f},{4,0,-10.5f},
        {-5,0,-11},{9,0,-4.5f},{-8,0,-8},{7.5f,0,-7.5f},{-2,0,-12.5f},{5.5f,0,-12.5f}
    };
    const float rs[] = {0.4f,0.6f,0.35f,0.7f,0.5f,0.45f, 0.55f,0.4f,0.65f,0.5f,0.42f,0.6f};
    for (int i = 0; i < 12; ++i) {
        glPushMatrix();
        glTranslatef(rb[i][0], rs[i]*0.4f, rb[i][2]);
        glRotatef(rb[i][0]*9.f, 0,1,0);
        drawBox(rs[i], rs[i]*0.8f, rs[i]);
        glPopMatrix();
    }
    // Broken poles / girders lying on the sand
    glColor3f(0.45f, 0.42f, 0.38f);
    for (int i = 0; i < 5; ++i) {
        float px = -6.f + i*3.f;
        glPushMatrix();
        glTranslatef(px, 0.07f, -3.8f + i*0.6f);
        glRotatef(20.f + i*32.f, 0,1,0);
        drawBox(1.7f, 0.10f, 0.10f);
        glPopMatrix();
    }
}

void Arena::draw() {
    drawSky();
    drawGround();
    drawRuins();
    drawDust();
}
