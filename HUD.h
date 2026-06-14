#pragma once
#include "platform_gl.h"

class Character;

enum class GameState { FIGHTING, GAME_OVER };

class HUD {
public:
    void draw(const Character& p1, const Character& p2,
              int winW, int winH, GameState state, float p1HealFrac);

private:
    void drawHpBar(float x, float y, float w, float h,
                   float frac, float r, float g, float b);
    void drawString(float x, float y, const char* str);
};
