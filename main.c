#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

/* Global texture variables */
GLuint texWood, texLeaf, texWall, texRoof, texCactus, texDirt, texCobblestone, texPlanks_oak;

/* Global Variables */
double ex = 0.0, ez = 0.0;    // Current position
double r  = 0.0;              // Direction (degrees)
double v  = 0.0;              // Forward speed
double a  = 0.0;              // Rotation speed
double centerX, centerY;      // Window center
double height = -2.0;          // 視点の高さ

/* Function Prototypes */
void tree(double x, double y, double z);
void cube(GLfloat *color, double x, double y, double z);
void drawGround(void);
void house(double x, double y, double z);
void house2(double x, double y, double z);
void world(void);
void cactus(double x, double y, double z);

void scene(void);
void display(void);
void resize(int w, int h);
void keyboard(unsigned char key, int x, int y);
void drag(int x, int y);
void idle(void);
void init(void);

void loadTextures(void);
void loadTexture(GLuint *texture, const char *filename);

void drawTexturedCube(void);

/* ================= Texture Loading ================= */

void loadTexture(GLuint *texture, const char *filename)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "Failed to load texture file %s\n", filename);
        exit(1);
    }

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    GLenum format = GL_RGB;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
}

void loadTextures(void)
{
    loadTexture(&texWood, "./wood.png");
    loadTexture(&texLeaf, "./leaf.png");
    loadTexture(&texWall, "./wall.png");
    loadTexture(&texRoof, "./roof.png");
    loadTexture(&texCactus, "./cactus.png");
    loadTexture(&texDirt, "./dirt.png");
    loadTexture(&texCobblestone, "./cobblestone.png");
    loadTexture(&texPlanks_oak, "./planks_oak.png");
}

/* ================= Initialization ================= */

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_TEXTURE_2D);
    loadTextures();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_LIGHTING);
}

/* ================= Drawing Functions ================= */

void drawTexturedCube(void)
{
    // Draw a cube centered at origin with side length 1
    // Each face has texture coordinates from (0,0) to (1,1)

    glBegin(GL_QUADS);
    // Front face (z = 0.5)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);

    // Back face (z = -0.5)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);

    // Left face (x = -0.5)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);

    // Right face (x = 0.5)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);

    // Top face (y = 0.5)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);

    // Bottom face (y = -0.5)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glEnd();
}

void tree(double x, double y, double z)
{
    glPushMatrix();
    glTranslated(x, y, z);

    // Draw trunk (3 cubes tall) with wood texture
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texWood);
    for (int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(0.0, h * 1.0, 0.0);
        drawTexturedCube();
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);

    // Draw leaves (3x3x3 cubes centered on top of trunk) with leaf texture
    glMaterialfv(GL_FRONT, GL_DIFFUSE, leafGreen);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texLeaf);
    for (int xx = -1; xx <= 1; xx++) {
        for (int yy = 3; yy <= 5; yy++) {
            for (int zz = -1; zz <= 1; zz++) {
                glPushMatrix();
                glTranslated(xx * 1.0, yy * 1.0, zz * 1.0);
                drawTexturedCube();
                glPopMatrix();
            }
        }
    }
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void cube(GLfloat *color, double x, double y, double z)
{
    glPushMatrix();
    glTranslated(x, y, z);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    drawTexturedCube();
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
    glEnable(GL_TEXTURE_2D);

    // 外壁の石
    glBindTexture(GL_TEXTURE_2D, texCobblestone);
    for (int h = 0; h < 3; h++) {
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                if((i == -2 && j == -2) || (i == -2 && j == 2) || (i == 2 && j == -2) || (i == 2 && j == 2)) continue;

                glPushMatrix();
                glTranslated(i * 1.0, h * 1.0, j * 1.0);
                drawTexturedCube();
                glPopMatrix();
            }
        }
    }
    
    // 外壁の木
    glBindTexture(GL_TEXTURE_2D, texWood);
    for(int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(-2.0, h * 1.0, -2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(-2.0, h * 1.0, 2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(2.0, h * 1.0, -2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(2.0, h * 1.0, 2.0);
        drawTexturedCube();
        glPopMatrix();
    }


    // 屋根
    glBindTexture(GL_TEXTURE_2D, texPlanks_oak);
    for (int layer = 0; layer < 4; layer++) {
        int size = 3 - layer;
        for (int i = -size; i <= size; i++) {
            for (int j = -size; j <= size; j++) {
                glPushMatrix();
                glTranslated(i * 1.0, 3.0 + layer, j * 1.0);
                drawTexturedCube();
                glPopMatrix();
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void house2(double x, double y, double z) {

    glPushMatrix();
    glTranslated(x, y, z);
    glEnable(GL_TEXTURE_2D);

    // 土台
    glBindTexture(GL_TEXTURE_2D, texCobblestone);
    for(int i = -2; i <= 2; i++) {
        for(int j = -2; j <= 2; j++) {
            glPushMatrix();
            glTranslated(i * 1.0, 0.0, j * 1.0);
            drawTexturedCube();
            glPopMatrix();
        }
    }

    // 外壁の木壁
    glBindTexture(GL_TEXTURE_2D, texPlanks_oak);
    for (int h = 0; h < 3; h++) {
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                if((i == -2 && j == -2) || (i == -2 && j == 2) || (i == 2 && j == -2) || (i == 2 && j == 2)) continue;

                glPushMatrix();
                glTranslated(i * 1.0, 1.0 + h * 1.0, j * 1.0);
                drawTexturedCube();
                glPopMatrix();
            }
        }
    }
    
    // 外壁の石
    glBindTexture(GL_TEXTURE_2D, texCobblestone);
    for(int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(-2.0, 1.0 + h * 1.0, -2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(-2.0, 1.0 + h * 1.0, 2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(2.0, 1.0 + h * 1.0, -2.0);
        drawTexturedCube();
        glPopMatrix();

        glPushMatrix();
        glTranslated(2.0, 1.0 + h * 1.0, 2.0);
        drawTexturedCube();
        glPopMatrix();
    }


    // 屋根
    for(int i = -2; i <= 2; i++) {
        for(int j = -2; j <= 2; j++) {
            if(i == -2 || i == 2 || j == -2 || j == 2) {
                glBindTexture(GL_TEXTURE_2D, texWood);
                glPushMatrix();
                glTranslated(i * 1.0, 4.0, j * 1.0);
                drawTexturedCube();
                glPopMatrix();
            } else {
                glBindTexture(GL_TEXTURE_2D, texPlanks_oak);
                glPushMatrix();
                glTranslated(i * 1.0, 4.0, j * 1.0);
                drawTexturedCube();
                glPopMatrix();
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void world(void)
{
    glEnable(GL_TEXTURE_2D);
    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            // glMaterialfv(GL_FRONT, GL_DIFFUSE, groundColor[(i + j) & 1]);
            glBindTexture(GL_TEXTURE_2D, texDirt);
            glPushMatrix();
            glTranslated(i * 1.0, 0.0, j * 1.0);
            drawTexturedCube();
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void cactus(double x, double y, double z) {
    glPushMatrix();
    glTranslated(x, y, z); 
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, leafGreen);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texCactus);
    for (int h = 0; h < 3; h++) {
        glPushMatrix();
        glTranslated(0.0, h * 1.0, 0.0);
        drawTexturedCube();
        glPopMatrix();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/* ================= Scene Setup ================= */

void scene(void)
{
    // Cubes
    // cube(red,    0.0, 1.0, -3.0);

    // Trees
    tree(-4.0, 1.0, 5.0);
    tree(4.0, 1.0, 4.0);

    // サボテン
    cactus(5.0, 1.0, -2.0);
    cactus(4.0, 1.0, -5.0);

    // 家
    house(-3.0, 1.0, -3.0);

    // house2
    house2(5.0, 1.0, 5.0);

    // 地面
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
        if(height > -1.0) break;
        height += 0.2;
        break;
    case 'w':
        height -= 0.2;
        break;
    default:
        break;
    }
}

void drag(int x, int y)
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
    ez -= v * cos(rad) * t;
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
    glutMotionFunc(drag);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
    return 0;
}