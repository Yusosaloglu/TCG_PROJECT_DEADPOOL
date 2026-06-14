#include "platform_gl.h"
#include "Game.h"

static Game world;

static void displayCB()              { world.draw(); glutSwapBuffers(); }
static void reshapeCB(int w, int h)  { world.reshape(w, h); }
static void timerCB(int) {
    world.tick();
    glutPostRedisplay();
    glutTimerFunc(16, timerCB, 0);   // ~60 fps
}
static void keyDownCB(unsigned char k, int x, int y) { world.keyDown(k, x, y); }
static void keyUpCB  (unsigned char k, int x, int y) { world.keyUp  (k, x, y); }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Blind Box Battle - TCG6223");

    world.init();

    glutDisplayFunc(displayCB);
    glutReshapeFunc(reshapeCB);    // registers gluPerspective — do not remove
    glutKeyboardFunc(keyDownCB);
    glutKeyboardUpFunc(keyUpCB);   // freeglut only; enables held-key movement

    glutTimerFunc(16, timerCB, 0);
    glutMainLoop();
    return 0;
}
