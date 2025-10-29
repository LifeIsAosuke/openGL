#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

/* Color Definitions */
static GLfloat red[]        = { 0.8, 0.2, 0.2, 1.0 };
static GLfloat green[]      = { 0.2, 0.8, 0.2, 1.0 };
static GLfloat blue[]       = { 0.2, 0.2, 0.8, 1.0 };
static GLfloat yellow[]     = { 0.8, 0.8, 0.2, 1.0 };
static GLfloat groundColor[][4] = {
    { 0.6, 0.6, 0.6, 1.0 },
    { 0.3, 0.3, 0.3, 1.0 }
};
static GLfloat brown[]      = { 0.55, 0.27, 0.07, 1.0 };
static GLfloat leafGreen[]  = { 0.2, 0.8, 0.2, 1.0 };
static GLfloat wallColor[]  = { 0.7, 0.4, 0.2, 1.0 };
static GLfloat roofColor[]  = { 0.8, 0.1, 0.1, 1.0 };

/* Global Variables */
double ex = 0.0, ez = 0.0;    // Current position
double r  = 0.0;              // Direction (degrees)
double v  = 0.0;              // Forward speed
double a  = 0.0;              // Rotation speed
double centerX, centerY;      // Window center
double height = -3.0;          // 視点の高さ

/* Function Prototypes */
void tree(double x, double y, double z);
void cube(GLfloat *color, double x, double y, double z);
void drawGround(void);
void house(double x, double y, double z);
void world(void);
void cactus(double x, double y, double z); // サボテン

void scene(void);
void display(void);
void resize(int w, int h);
void keyboard(unsigned char key, int x, int y);
void drug(int x, int y);
void idle(void);
void init(void);

/* ================= Initialization ================= */

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

/* ================= Drawing Functions ================= */

void tree(double x, double y, double z)
{
    glPushMatrix();
    glTranslated(x, y, z);

    // Draw trunk (3 cubes tall)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
    for (int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(0.0, h * 1.0, 0.0);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // Draw leaves (3x3x3 cubes centered on top of trunk)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leafGreen);
    for (int x = -1; x <= 1; x++) {
        for (int y = 3; y <= 5; y++) {
            for (int z = -1; z <= 1; z++) {
                glPushMatrix();
                glTranslated(x * 1.0, y * 1.0, z * 1.0);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    glPopMatrix();
}

void cube(GLfloat *color, double x, double y, double z)
{
    glPushMatrix();
    glTranslated(x, y, z);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawGround(void)
{
    int i, j;
    glBegin(GL_QUADS);
    glNormal3d(0.0, 1.0, 0.0);
    for (j = -5; j < 5; ++j) {
        for (i = -5; i < 5; ++i) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, groundColor[(i + j) & 1]);
            glVertex3d(i, -0.5, j);
            glVertex3d(i, -0.5, j + 1);
            glVertex3d(i + 1, -0.5, j + 1);
            glVertex3d(i + 1, -0.5, j);
        }
    }
    glEnd();
}

void house(double x, double y, double z)
{
    glPushMatrix();
    glTranslated(x, y, z);

    // Walls (3x3x2 blocks)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wallColor);
    for (int h = 0; h < 2; h++) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                glPushMatrix();
                glTranslated(i * 1.0, h * 1.0, j * 1.0);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    // Roof (3 layers, stair-like)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, roofColor);
    for (int layer = 0; layer < 3; layer++) {
        int size = 2 - layer;
        for (int i = -size; i <= size; i++) {
            for (int j = -size; j <= size; j++) {
                glPushMatrix();
                glTranslated(i * 1.0, 2.0 + layer, j * 1.0);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    glPopMatrix();
}

void world(void)
{
    for (int h = 0; h < 12; h++) {
        for (int i = -6; i < 6; i++) {
            for (int j = -6; j < 6; j++) {
                glMaterialfv(GL_FRONT, GL_DIFFUSE, groundColor[(i + j) & 1]);
                glPushMatrix();
                glTranslated(i * 1.0, -h * 1.0, j * 1.0);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }
}

void cactus(double x, double y, double z) {
    glPushMatrix();
    glTranslated(x, y, z); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leafGreen);
    for (int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(0.0, h * 1.0, 0.0);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glPopMatrix();
}

/* ================= Scene Setup ================= */

void scene(void)
{
    // Cubes
    // cube(red,    0.0, 1.0, -3.0);
    // cube(green,  0.0, 1.0,  3.0);
    // cube(blue,  -3.0, 1.0,  0.0);
    // cube(yellow, 0.0, 1.0,  0.0);

    // Trees
    tree(-4.0, 1.0, 5.0);
    tree(4.0, 1.0, 4.0);

    // サボテン
    cactus(5.0, 1.0, -2.0);
    cactus(4.0, 1.0, -5.0);

    // Fallen tree
    // glPushMatrix();
    // glTranslated(-4.0, 1.0, 0.0);
    // glRotated(90.0, 0.0, 0.0, 1.0); // Rotate 90 degrees around Z-axis to fall
    // tree(0.0, 0.0, 0.0);
    // glPopMatrix();

    // House
    house(-3.0, 1.0, -3.0);

    // Ground
    // drawGround();

    // World blocks
    world();
}

/* ================= Camera Controls ================= */

void display(void)
{
    static GLfloat lightpos[] = { 3.0, 4.0, -5.0, 1.0 };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotated(r, 0.0, 1.0, 0.0);
    glTranslated(-ex, height, -ez);

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    scene();

    glFlush();
}

void resize(int w, int h)
{
    centerX = w / 2.0;
    centerY = h / 2.0;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    // Exit on 'q' or ESC
    if (key == '\033' || key == 'q') {
        exit(0);
    }
    switch (key) {
    case 's':
        height += 0.5;
        break;
    case 'w':
        height -= 0.5;
        break;
    default:
        break;
    }
}

void drug(int x, int y)
{
    int dx = x - centerX;
    int dy = y - centerY;

    double vscale = 0.01;
    double ascale = 0.1;

    v = -dy * vscale;
    a = dx * ascale;
}

void idle(void)
{
    double t = 0.05;
    double rad = r * M_PI / 180.0;

    ex += v * sin(rad) * t;
    ez += v * cos(rad) * t;
    r += a * t;

    glutPostRedisplay();
}

/* ================= Main ================= */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(drug);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
    return 0;
}