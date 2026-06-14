#pragma once
#include "platform_gl.h"
#include "Constants.h"
#include "Character.h"
#include "Wolverine.h"
#include "Deadpool.h"
#include "Arena.h"
#include "Particles.h"
#include "HUD.h"

class Game {
public:
    void init();

    void draw();
    void tick();

    void reshape(int w, int h);
    void keyDown(unsigned char k, int x, int y);
    void keyUp  (unsigned char k, int x, int y);

private:
    Wolverine      p1;
    Deadpool       p2;
    Arena          arena;
    ParticleSystem particles;
    HUD            hud;

    bool      keyState[256] = {};
    long      lastTime      = 0;
    int       winW          = 800;
    int       winH          = 600;
    GameState gameState     = GameState::FIGHTING;

    void moveCharacter(Character& ch,
                       unsigned char fwd, unsigned char back,
                       unsigned char left, unsigned char right,
                       unsigned char turnL, unsigned char turnR,
                       float dt);

    void resetMatch();
};
