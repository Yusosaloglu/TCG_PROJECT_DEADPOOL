#include "Game.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>
#include <cstring>

// ── Init ──────────────────────────────────────────────────────────────────────
void Game::init() {
    // Core GL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);    // auto-normalise scaled normals (required for drawTrapezoidBody)
    glShadeModel(GL_SMOOTH);

    // Colour material — glColor3f sets AMBIENT+DIFFUSE on front faces
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // GL_LIGHT0 properties set here; position is set each frame in draw()
    // after gluLookAt so the directional light is in correct world space.
    GLfloat diff0[]  = {0.90f, 0.85f, 0.75f, 1.f};  // warm sun
    GLfloat amb0[]   = {0.25f, 0.22f, 0.18f, 1.f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff0);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb0);

    // GL_LIGHT1 = impact flash (white, brief)
    // GL_LIGHT2 = rage rim (red, sustained) — separate slots to avoid stomping
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);

    glClearColor(0.3f, 0.3f, 0.35f, 1.f);

    arena.init();

    lastTime = glutGet(GLUT_ELAPSED_TIME);
}

// ── Reshape / camera projection ───────────────────────────────────────────────
void Game::reshape(int w, int h) {
    winW = w;
    winH = (h > 0) ? h : 1;
    glViewport(0, 0, winW, winH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(CAMERA_FOV, (double)winW / (double)winH, NEAR_PLANE, FAR_PLANE);
    glMatrixMode(GL_MODELVIEW);
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void Game::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.f, CAMERA_HEIGHT, CAMERA_DIST,   // eye
              0.f, 0.f,           0.f,            // look-at
              0.f, 1.f,           0.f);            // up

    // Sun position — must be set after gluLookAt (transforms to eye space)
    GLfloat sunPos[] = {5.f, 10.f, 5.f, 0.f};    // w=0 → directional
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);

    // Impact flash on GL_LIGHT1
    if (arena.impactFlash > 0.f) {
        GLfloat fpos[]  = {0.f, 6.f, 0.f, 0.f};
        GLfloat white[] = {1.f, 1.f, 1.f, 1.f};
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  white);
        glLightfv(GL_LIGHT1, GL_POSITION, fpos);
        glEnable(GL_LIGHT1);
    } else {
        glDisable(GL_LIGHT1);
    }

    // Rage rim on GL_LIGHT2 (when either fighter near death)
    bool rage = (p1.hp / MAX_HP < ENV_RAGE_THRESHOLD && p1.alive) ||
                (p2.hp / MAX_HP < ENV_RAGE_THRESHOLD && p2.alive);
    if (rage && gameState == GameState::FIGHTING) {
        GLfloat rpos[] = {0.f, -3.f, 2.f, 0.f};
        GLfloat red[]  = {0.60f, 0.04f, 0.04f, 1.f};
        glLightfv(GL_LIGHT2, GL_DIFFUSE,  red);
        glLightfv(GL_LIGHT2, GL_POSITION, rpos);
        glEnable(GL_LIGHT2);
    } else {
        glDisable(GL_LIGHT2);
    }

    arena.draw();
    p1.draw();
    p2.draw();
    particles.draw();
    hud.draw(p1, p2, winW, winH, gameState);
}

// ── Tick ──────────────────────────────────────────────────────────────────────
void Game::tick() {
    long now = glutGet(GLUT_ELAPSED_TIME);
    float dt  = (float)(now - lastTime) * 0.001f;
    lastTime  = now;
    if (dt > 0.1f) dt = 0.1f;  // clamp: handles window-resize stalls

    if (gameState != GameState::FIGHTING) return;

    // Movement
    moveCharacter(p1, 'w','s','a','d','q','e', dt);
    moveCharacter(p2, 'i','k','j','l','u','o', dt);

    // Skill ticks (animation + hit detection → events)
    CharEvent e1 = p1.tick(dt, p2);
    CharEvent e2 = p2.tick(dt, p1);

    // Consume particle emit requests
    for (int i = 0; i < e1.emitCount; ++i) {
        const EmitRequest& em = e1.emits[i];
        particles.emit(em.x,em.y,em.z, em.r,em.g,em.b, em.count,em.speed);
    }
    for (int i = 0; i < e2.emitCount; ++i) {
        const EmitRequest& em = e2.emits[i];
        particles.emit(em.x,em.y,em.z, em.r,em.g,em.b, em.count,em.speed);
    }
    if (e1.doImpactFlash || e2.doImpactFlash)
        arena.impactFlash = ENV_IMPACT_FLASH_DUR;

    // Min-separation (resolved once at world level — NOT inside Character::tick)
    float dx   = p2.x - p1.x, dz = p2.z - p1.z;
    float dist = sqrtf(dx*dx + dz*dz);
    if (dist < MIN_SEPARATION && dist > 0.001f) {
        float push = (MIN_SEPARATION - dist) * 0.5f;
        p1.x -= dx/dist * push;  p1.z -= dz/dist * push;
        p2.x += dx/dist * push;  p2.z += dz/dist * push;
    }

    // Environment intensity (0 = fresh, 1 = both near death)
    float t = 1.f - (p1.hp + p2.hp) / (2.f * MAX_HP);
    arena.setEnvironmentIntensity(t);
    arena.tick(dt);

    particles.tick(dt);

    // Win condition
    if (!p1.alive || !p2.alive)
        gameState = GameState::GAME_OVER;
}

// ── Input ─────────────────────────────────────────────────────────────────────
void Game::keyDown(unsigned char k, int, int) {
    k = (unsigned char)tolower((int)k);
    keyState[k] = true;

    if (k == 27) std::exit(0);  // ESC

    if (gameState == GameState::FIGHTING) {
        if (k == 'z') p1.startSkill1();
        if (k == 'x') p1.startSkill2();
        if (k == 'n') p2.startSkill1();
        if (k == 'm') p2.startSkill2();
    }
    if (k == 'r' && gameState == GameState::GAME_OVER)
        resetMatch();
}

void Game::keyUp(unsigned char k, int, int) {
    keyState[tolower((int)k)] = false;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
void Game::moveCharacter(Character& ch,
                         unsigned char fwd,  unsigned char back,
                         unsigned char left, unsigned char right,
                         unsigned char tL,   unsigned char tR,
                         float dt) {
    if (!ch.alive) return;

    if (keyState[tL]) ch.facingAngle -= PLAYER_TURN * dt;
    if (keyState[tR]) ch.facingAngle += PLAYER_TURN * dt;

    float rad = ch.facingAngle * (float)M_PI / 180.f;
    float spd = PLAYER_SPEED * dt;

    if (keyState[fwd])  { ch.x += sinf(rad)*spd; ch.z += cosf(rad)*spd; }
    if (keyState[back]) { ch.x -= sinf(rad)*spd; ch.z -= cosf(rad)*spd; }
    if (keyState[left]) { ch.x -= cosf(rad)*spd; ch.z += sinf(rad)*spd; }
    if (keyState[right]){ ch.x += cosf(rad)*spd; ch.z -= sinf(rad)*spd; }

    ch.x = clampf(ch.x, -ARENA_HALF, ARENA_HALF);
    ch.z = clampf(ch.z, -ARENA_HALF, ARENA_HALF);
}

void Game::resetMatch() {
    p1 = Wolverine();
    p2 = Deadpool();
    particles = ParticleSystem();
    arena.impactFlash = 0.f;
    gameState = GameState::FIGHTING;
    lastTime  = glutGet(GLUT_ELAPSED_TIME);
}
