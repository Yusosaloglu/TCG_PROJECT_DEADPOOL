#include "HUD.h"
#include "Character.h"
#include "Constants.h"

void HUD::drawHpBar(float x, float y, float w, float h,
                    float frac, float r, float g, float b) {
    // Background (dark)
    glColor3f(0.18f, 0.18f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);       glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);   glVertex2f(x,   y+h);
    glEnd();
    // Filled portion
    glColor3f(r, g, b);
    float fw = w * frac;
    glBegin(GL_QUADS);
    glVertex2f(x,    y);     glVertex2f(x+fw, y);
    glVertex2f(x+fw, y+h);  glVertex2f(x,    y+h);
    glEnd();
    // Border
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);       glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);   glVertex2f(x,   y+h);
    glEnd();
}

void HUD::drawString(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    for (const char* c = str; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
}

void HUD::draw(const Character& p1, const Character& p2,
               int winW, int winH, GameState state, float p1HealFrac) {
    // Switch to 2D ortho overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    float barW = (float)winW * 0.28f;
    float barH = 20.f;
    float barY = (float)winH - 46.f;

    // P1 HP bar (red, top-left) + Berserker Heal gauge beneath it
    drawHpBar(18.f, barY, barW, barH, p1.hp / MAX_HP, 0.9f, 0.18f, 0.18f);
    drawHpBar(18.f, barY - 8.f, barW, 5.f, p1HealFrac, 0.25f, 0.95f, 0.45f);  // heal charge
    glColor3f(0.95f, 0.95f, 0.95f);
    drawString(20.f, (float)winH - 70.f, "WOLVERINE  (X: heal)");

    // P2 HP bar (blue, top-right)
    float p2x = (float)winW - barW - 18.f;
    drawHpBar(p2x, barY, barW, barH, p2.hp / MAX_HP, 0.18f, 0.40f, 0.90f);
    glColor3f(0.95f, 0.95f, 0.95f);
    drawString(p2x, (float)winH - 56.f, "DEADPOOL");

    // Controls reminder (small, bottom)
    glColor3f(0.55f, 0.00f, 0.00f);
    drawString(10.f, 10.f, "P1: W/S move  Q/E turn  A/D roll  Z=Claw  X=Heal     P2: I/K move  U/O turn  J/L roll  N=Katana  M=Desert Eagle     R=Restart");

    // Win overlay
    if (state == GameState::GAME_OVER) {
        const char* msg = !p1.alive ? "DEADPOOL WINS!" : "WOLVERINE WINS!";
        glColor3f(1.f, 0.85f, 0.f);
        // Simple centred text (character width ~7px with HELVETICA_18)
        int len = 0; for (const char* c = msg; *c; ++c) ++len;
        float tx = (float)winW * 0.5f - len * 5.5f;
        glRasterPos2f(tx, (float)winH * 0.5f);
        for (const char* c = msg; *c; ++c)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

        glColor3f(0.75f, 0.75f, 0.75f);
        drawString((float)winW*0.5f - 42.f, (float)winH*0.5f - 22.f, "Press R to restart");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
