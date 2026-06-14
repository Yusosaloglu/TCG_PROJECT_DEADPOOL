#pragma once
#include "platform_gl.h"

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life, maxLife;
    float size;
    float r, g, b;
};

class ParticleSystem {
public:
    static const int MAX = 512;

    void emit(float x, float y, float z,
              float r, float g, float b,
              int count, float speed);

    void tick(float dt);
    void draw();

private:
    Particle pool[MAX];
    int      count = 0;
};
