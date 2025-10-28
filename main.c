#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

double ex = 0.0, ez = 0.0; // 現在位置
double r  = 0.0;           // 向き（度）
double v  = 0.0;           // 進行速度
double a  = 0.0;           // 回転速度
double centerX, centerY;   // ウィンドウ中心
static int lastX, lastY;

/* 物体の色 */
static GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
static GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };
static GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
static GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };
static GLfloat groundColor[][4] = {
  { 0.6, 0.6, 0.6, 1.0 },
  { 0.3, 0.3, 0.3, 1.0 }
};

// 木 (Minecraft-style tree)
static GLfloat brown[] = {0.55, 0.27, 0.07, 1.0};
static GLfloat leafGreen[] = {0.2, 0.8, 0.2, 1.0};

// オブジェクトのプロトタイプ宣言
void tree(double x, double y, double z);
void cube(GLfloat *color, double x, double y, double z);
void ground(void);
void house(double x, double y, double z);

void scene(void)
{
  // 箱オブジェクト群
  cube(red,    0.0, 0.0, -3.0);
  cube(green,  0.0, 0.0,  3.0);
  cube(blue,  -3.0, 0.0,  0.0);
  cube(yellow, 3.0, 0.0,  0.0);

  // 木オブジェクト
  tree(4.0, 0.0, 0.0);
  tree(-4.0, 0.0, 0.0);

  // 倒れた木の描画
  glPushMatrix();
  glTranslated(-4.0, 0.0, 0.0);
  glRotated(90.0, 0.0, 0.0, 1.0); // X軸に沿って倒すためZ軸回転90度
  tree(0.0, 0.0, 0.0);
  glPopMatrix();


  // 家
  house(8.0, 0.0, 0.0);

  // 地面
  ground();
}

void display(void)
{
  static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* 光源の位置 */

  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* モデルビュー変換行列の初期化 */
  glLoadIdentity();

  /* 視点の移動 */
  glRotated(r, 0.0, 1.0, 0.0);
  glTranslated(-ex, 0.0, -ez); // 視点の位置を反転

  /* 光源の位置を設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  /* シーンの描画 */
  scene();

  glFlush();
}

void resize(int w, int h)
{
  // ウィンドウの中心位置を求める
  centerX = w/2.0;
  centerY = h/2.0;

  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);

  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  /* モデルビュー変換行列の指定 */
  glMatrixMode(GL_MODELVIEW);

}

void keyboard(unsigned char key, int x, int y)
{
  /* ESC か q をタイプしたら終了 */
  if (key == '\033' || key == 'q') {
    exit(0);
  }
}

// マウスドラッグ処理
void drug(int x, int y) {
    int dx = x - centerX; // 右方向の変位
    int dy = y - centerY; // 上方向の変位

    // スケーリング係数
    double vscale = 0.01;
    double ascale = 0.1;
    
    // 前後速度（dyが上方向＝マイナスで前進）
    v = -dy * vscale;

    // 回転速度（右方向がプラス）
    a = dx * ascale;
}

// 視点更新
void idle() {
    double t = 0.05; // 時間刻み
    double rad = r * M_PI / 180.0; // ラジアン変換

    // 位置更新
    ex += v * sin(rad) * t;
    ez += v * cos(rad) * t;
    r += a * t;

    glutPostRedisplay();
}

void init(void)
{
  /* 初期設定 */
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

// 木
void tree(double x, double y, double z) {
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

// 立方体
void cube(GLfloat *color, double x, double y, double z) {
  glPushMatrix();
  glTranslated(x, y, z);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glutSolidCube(1.0);
  glPopMatrix();
}

// 地面
void ground(void) {
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

void house(double x, double y, double z) {
    glPushMatrix();
    glTranslated(x, y, z);

    GLfloat wall[] = {0.7, 0.4, 0.2, 1.0};
    GLfloat roof[] = {0.8, 0.1, 0.1, 1.0};

    // 壁 (3x3x2ブロック)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall);
    for(int h = 0; h < 2; h++) {       // 高さ2ブロック
        for(int i = -1; i <= 1; i++) { // X方向
            for(int j = -1; j <= 1; j++) { // Z方向
                glPushMatrix();
                glTranslated(i*1.0, h*1.0, j*1.0); // +0.5でブロック中心
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    // 屋根（階段状3段）
    glMaterialfv(GL_FRONT, GL_DIFFUSE, roof);
    for(int layer = 0; layer < 3; layer++) {
        int size = 2 - layer; // 層ごとに幅を狭める
        for(int i = -size; i <= size; i++) {
            for(int j = -size; j <= size; j++) {
                glPushMatrix();
                glTranslated(i*1.0, 2.0 + layer, j*1.0);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    glPopMatrix();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(drug); // ドラッグ関数登録
  glutIdleFunc(idle); // 更新関数登録
  init();
  glutMainLoop();
  return 0;
}