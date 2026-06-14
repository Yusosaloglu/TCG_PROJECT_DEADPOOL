#include "Particles.h"
#include <cstdlib>
#include <cmath>

static float frand() { return (float)rand() / (float)RAND_MAX; }

void ParticleSystem::emit(float x, float y, float z,
                          float r, float g, float b,
                          int count, float speed) {
    for (int i = 0; i < count; ++i) {
        // Recycle oldest particle if pool is full
        int slot = (this->count < MAX) ? this->count++ : (MAX - 1);
        Particle& p = pool[slot];
        // Random unit direction
        float theta = frand() * 2.f * 3.14159f;
        float phi   = (frand() - 0.5f) * 3.14159f;
        p.vx = cosf(phi) * cosf(theta) * speed;
        p.vy = cosf(phi) * sinf(theta) * speed;
        p.vz = sinf(phi)               * speed;
        p.vy = fabsf(p.vy) + speed * 0.3f;  // bias upward
        p.x  = x; p.y = y; p.z = z;
        p.r  = r + (frand()-0.5f)*0.15f;
        p.g  = g + (frand()-0.5f)*0.15f;
        p.b  = b + (frand()-0.5f)*0.15f;
        p.life = p.maxLife = 0.4f + frand() * 0.4f;
        p.size = 0.06f + frand() * 0.06f;
    }
}

void ParticleSystem::tick(float dt) {
    for (int i = 0; i < count; ) {
        pool[i].life -= dt;
        pool[i].x    += pool[i].vx * dt;
        pool[i].y    += pool[i].vy * dt;
        pool[i].z    += pool[i].vz * dt;
        pool[i].vy   -= 2.f * dt;  // gravity
        if (pool[i].life <= 0.f) {
            pool[i] = pool[--count];  // swap-remove (fast, order-independent)
        } else {
            ++i;
        }
    }
}

void ParticleSystem::draw() {
    if (count == 0) return;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   // additive — bright glow effect
    glDepthMask(GL_FALSE);

    for (int i = 0; i < count; ++i) {
        const Particle& p = pool[i];
        float alpha = (p.life / p.maxLife);
        glColor4f(p.r, p.g, p.b, alpha);
        glPointSize(p.size * alpha * 18.f);
        glBegin(GL_POINTS);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
