//  ======================================
//
//  COSC363: assignment 1
//  31/03/2023
//
//  ======================================

#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <cmath>
#include <GL/freeglut.h>
#include "C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\loadTGA.h"

using namespace std;

// GLOBALS
float* x, * y, * z;                 // vertex coordinates
int* nv, * t1, * t2, * t3, * t4;    // number of vertices and vertex indices of each face
int nvert, nface;                   // total number of vertices and faces
float xmin, xmax, ymin, ymax;       // min, max values of  object coordinates

float xpts[25] = { -160, -145, -140, -120, -110, -100, -75, -50, -25, -10, -5, -1, 0, 1, 5, 10, 25, 50, 75, 100, 110, 120, 140, 145, 160};
float zpts[25] = { -10, -5, 0, 5, 10, 5, 0, -5, -10, -5, 0, 5, 10, 5, 0, -5, -10, -5, 0, 5, 10, 5, 0, -5, 10};

GLuint txId[5];                     // texture IDs

float angle = 0, lookX = 0, lookZ = 0., eyeX = 0, eyeZ = 30;    // view params

int counter = 0;                    // timer variables
float amesTheta = 0;
float ringAngle = 0;
float sphereSize = 1;
float scanX = 0;
int sphereDirection = 1;
int scanDirection = 1;

// loads mesh data in OFF format (from Lab 01)
void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int ne;

    fp_in.open(fname, ios::in);
    if (!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');                // ignore first line
    fp_in >> nvert >> nface >> ne;              // read number of vertices, polygons, edges (not used)

    x = new float[nvert];                       // create arrays
    y = new float[nvert];
    z = new float[nvert];

    nv = new int[nface];
    t1 = new int[nface];
    t2 = new int[nface];
    t3 = new int[nface];
    t4 = new int[nface];

    for (int i = 0; i < nvert; i++)             // read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for (int i = 0; i < nface; i++)             // read polygon list
    {
        fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i];
        if (nv[i] == 4)
            fp_in >> t4[i];
        else
            t4[i] = -1;
    }

    fp_in.close();
    cout << " File successfully read." << endl;
}

// loads textures in TGA format (from Lab 03)
void loadTextures()
{
    glGenTextures(5, txId);                                             // create texture ids

    glBindTexture(GL_TEXTURE_2D, txId[0]);                              // scanimation
    loadTGA("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\scanimation.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, txId[1]);                              // scanimation lines
    loadTGA("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\scanimationLines.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, txId[2]);                              // rotating snakes static illusion
    loadTGA("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\rotatingSnakes.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[3]);                              // static illusion
    loadTGA("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\staticIllusion.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[4]);                              // background wall
    loadTGA("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\neonLines.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// function to compute the normal vector of a triangle with index indx (from Lab 01)
void normal(int indx)
{
    float x1 = x[t1[indx]], x2 = x[t2[indx]], x3 = x[t3[indx]];
    float y1 = y[t1[indx]], y2 = y[t2[indx]], y3 = y[t3[indx]];
    float z1 = z[t1[indx]], z2 = z[t2[indx]], z3 = z[t3[indx]];
    float nx, ny, nz;
    nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    glNormal3f(nx, ny, nz);
}

// keyboard event callback function
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':                   // AAO-1 view
        eyeX = -10;
        eyeZ = 30;
        angle = -1.6;
        break;
    case '2':                   // AAO-2 view
        eyeX = 0;
        eyeZ = -8;
        angle = 0;
        break;
    case '3':                   // AAO-3 view
        eyeX = 15;
        eyeZ = 30;
        angle = -4.7;
        break;
    case '4': 
        eyeX = 0;
        eyeZ = 30;
        angle = 0;
        break;
    case '0':                   // gallery view
        eyeX = 0;
        eyeZ = 160;
        angle = 0;
        break;
    }
    lookX = eyeX + 1 * sin(angle);
    lookZ = eyeZ - 1 * cos(angle);
    glutPostRedisplay();
}

// special keyboard event callback function (from Lab 04)
void special(int key, int x, int y)
{
    float eyeXsin = 0.7 * sin(angle);
    float eyeZcos = 0.7 * cos(angle);
    if (key == GLUT_KEY_LEFT) angle -= 0.1;         // turn left
    else if (key == GLUT_KEY_RIGHT) angle += 0.1;   // turn right
    else if (key == GLUT_KEY_DOWN && (eyeZ <= 150) && (eyeX <= (20 + eyeXsin)) && (eyeX >= (-20 + eyeXsin)))
    {
        eyeX -= eyeXsin;                            // move backwards
        eyeZ += eyeZcos;
    }
    else if (key == GLUT_KEY_UP && (eyeZ >= -8) && ((eyeX + eyeXsin) <= 20) && ((eyeX + eyeXsin) >= -20))
    {
        eyeX += eyeXsin;                            // move forwards
        eyeZ -= eyeZcos;
    }

    lookX = eyeX + 10 * sin(angle);
    lookZ = eyeZ - 10 * cos(angle);
    glutPostRedisplay();

}

// window reshape (from Lecture Slide [01]:45)
void reshape(int width, int height)
{
    float aspectCurr = (float)width / (float)height;
    float aspectReqd = 1;
    if (aspectCurr > aspectReqd)
        glViewport(0, 0, height * aspectReqd, height);
    else
        glViewport(0, 0, width, width / aspectReqd);
}

// timer
void timer(int value)
{
    // ames window
    amesTheta += 1.5;
    if (amesTheta > 360.0) amesTheta -= 360.0;

    // 3d model
    ringAngle += 1.5;
    if (ringAngle > 360.0) ringAngle -= 360.0;

    if (counter % 3 == 0)
    {
        sphereSize += (0.1 * sphereDirection);
        if ((sphereSize <= -0.5 && sphereDirection < 0) || (sphereSize >= 1.2 && sphereDirection > 0))
        {
            sphereDirection = -sphereDirection;
        }
    }

    if (counter % 2 == 0) {
        scanX += scanDirection;
        if ((scanX <= -20 && scanDirection < 0) || (scanX >= 20 && scanDirection > 0))
        {
            scanDirection = -scanDirection;
        }
    }

    counter++;
    glutTimerFunc(50, timer, 0);
    glutPostRedisplay();
}

// draws checkered floor plane (from Lab 02)
void drawFloor(void)
{
    bool flag = false;

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    for (int x = -300; x <= 300; x += 5)
    {
        for (int z = -300; z <= 300; z += 5)
        {
            if (flag) glColor3f(0.5, 0.5, 0.5);
            else glColor3f(0.8, 0.8, 0.8);

            glVertex3f(x, 0, z);
            glVertex3f(x, 0, z + 5);
            glVertex3f(x + 5, 0, z + 5);
            glVertex3f(x + 5, 0, z);

            flag = !flag;
        }
    }
    glEnd();
}

// draws curved wall (from Lab 04)
void drawCurvedWall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    glColor3f(1, 1, 1);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 25; i++)
    {
        glTexCoord2f((float)i / (float)(24), 0);
        glVertex3f(xpts[i], 0, zpts[i]);
        glTexCoord2f((float)i / (float)(24), 1);
        glVertex3f(xpts[i], 65, zpts[i]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draws walls and ceiling of room
void drawRoom(void)
{
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    // ceiling
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    // walls
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glEnd();
}

// draws mesh model of ames window
void drawAmesWindow()
{
    glColor3f(0.0, 1.0, 1.0);                                    // border
    for (int indx = 0; indx < (nface - 13); indx++)
    {
        normal(indx);
        glBegin(GL_QUADS);
        glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
        glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
        glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
        glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
        glEnd();
    }

    glColor3f(1.0, 0.0, 1.0);                                    // shadow
    for (int indx = 9; indx < nface; indx++)
    {
        normal(indx);
        glBegin(GL_QUADS);
        glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
        glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
        glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
        glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
        glEnd();
    }
}

void drawRings(void)
{
    glPushMatrix();
    glTranslatef(0, 10, -5);
    glRotatef(ringAngle, 1, 0, 1);
    glutSolidTorus(0.1, 2, 10, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(1.3, 1.3, 1.3);
    glRotatef((ringAngle + 60), 1, 0, 1);
    glutSolidTorus(0.1, 2, 10, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(1.5, 1.5, 1.5);
    glRotatef((ringAngle + 120), 1, 0, 1);
    glutSolidTorus(0.1, 2, 10, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(2, 2, 2);
    glRotatef(-ringAngle, 0, 0, 1);
    glutSolidTorus(0.1, 2, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(2.5, 2.5, 2.5);
    glRotatef(ringAngle, 0, 0, 1);
    glutSolidTorus(0.1, 2, 10, 6);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(3, 3, 3);
    glRotatef((ringAngle * -0.5), 0, 0, 1);
    glutSolidTorus(0.1, 2, 10, 4);
    glPopMatrix();
}

// draws 3D model made of GLUT objects
void draw3DModel(void)
{
    glColor3f(0.2, 0.2, 0.2);                           // platform
    glPushMatrix();
    glTranslatef(0, 0, -5);
    glScalef(2.5, 0.5, 2.5);
    glutSolidCube(5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 1, -5);
    glScalef(2, 2, 2);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.5, 2, 10, 20);
    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glColor4f(0.8, 0.1, 0.1, 0.5);
    glPushMatrix();
    glTranslatef(0, 2, -5);
    glScalef(4, 1, 4);
    glutSolidSphere(1, 36, 12);
    glPopMatrix();
    glDisable(GL_BLEND);

    float light1_pos[4] = { 0, 50, -10, 1 };             // spotlight and shadow
    float light1_dir[3] = { 0, -1, 0 };
    float shadowMat[16] = {
        light1_pos[1], 0, 0, 0, 
        -light1_pos[0], 0,
        -light1_pos[2], -1, 0, 0, 
        light1_pos[1], 0, 0, 0, 0, 
        light1_pos[1] 
    };
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);

    float white[3] = { 1, 1, 1 };
    float yellow[3] = { 1.0, 0.7, 0.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, yellow);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yellow);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 150);

    glColor3f(1.0, 0.7, 0.0);
    drawRings();
    glColor3f(1, 1, 1);                             // sphere
    glPushMatrix();
    glTranslatef(0, 10, -5);
    glScalef(sphereSize, sphereSize, sphereSize);
    glutSolidSphere(1, 36, 12);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.3, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(-11, 0.1, -10);
    glMultMatrixf(shadowMat);
    drawRings();
    glPushMatrix();
    glColor3f(0.2, 0.1, 0.1);
    glTranslatef(0, 10, -5);
    glScalef(sphereSize, sphereSize, sphereSize);
    glutSolidSphere(1, 36, 12);
    glPopMatrix();
    glPopMatrix();
}

// draw quad with scanimation pattern texture mapped
void drawScanimationPattern(void)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(-15, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3f(15, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(15, 20, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-15, 20, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draw quad with scanimation lines texture mapped
void drawScanimationLines(void)
{
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glPushMatrix();
        glTranslatef(scanX, 0, 0);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1);
            glVertex3f(-15, 0, 1);
            glTexCoord2f(1, 1);
            glVertex3f(15, 0, 1);
            glTexCoord2f(1, 0);
            glVertex3f(15, 20, 1);
            glTexCoord2f(0, 0);
            glVertex3f(-15, 20, 1);
        glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

// draws quads that are texture mapped with static optical illusions
void drawStaticIllusion(int txtI)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[txtI]);
    glColor3f(0., 1, 1.);
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);
    glTexCoord2f(1, 0);
    glVertex3f(20, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3f(20, 20, 0);
    glTexCoord2f(0, 1);
    glVertex3f(0, 20, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// display (generates scene)
void display(void)
{
    float light_pos[4] = { 0, 50.0, 10.0, 1.0 };

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                  // GL_LINE = Wireframe;   GL_FILL = Solid
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eyeX, 10, eyeZ, lookX, 10, lookZ, 0, 1, 0);       // camera position

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);               // light position

    glDisable(GL_LIGHTING);
    glPushMatrix();                                             // floor
    glRotatef(45, 0, 1, 0);
    drawFloor();
    glPopMatrix();

    glPushMatrix();                                             // walls and ceiling
    glTranslatef(0, 30, 0);
    glScalef(150, 30, 200);
    drawRoom();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, 0, -200);
        drawCurvedWall();
    glPopMatrix();

    glPushMatrix();                                             // ames window (AAO-1)
    glTranslatef(-50, 10, 33);
    glRotatef(amesTheta, 0, 1, 0);
    glTranslatef(-11.5, 0, 0);
    drawAmesWindow();
    glPopMatrix();

    glPushMatrix();                                             // scanimation (AAO-2)
    glTranslated(0, 0, -50);
    drawScanimationPattern();
    drawScanimationLines();
    glPopMatrix();

    glPushMatrix();                                             // static illusions
        glTranslatef(-40, 2, 6);
        glRotatef(45, 0, 1, 0);
        drawStaticIllusion(2);  
    glPopMatrix();

    glPushMatrix();
    glTranslatef(35, 2, -3);
    glRotatef(-45, 0, 1, 0);
    drawStaticIllusion(3);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPushMatrix();                                             // animated 3D model (AAO-3)
    glTranslatef(61.5, 0, 30);
    glRotatef(90, 0, 1, 0);
    draw3DModel();
    glPopMatrix();

    glFlush();
}

// initialize OpenGL parameters
void initialize(void)
{
    float red[] = { 1, 0, 0, 1 };

    loadMeshFile("C:\\Users\\jfu24\\Desktop\\cosc363-assignment1-main\\amesWindow.off");
    loadTextures();

    glClearColor(1.0, 1.0, 1.0, 1.0);       // background colour

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, red);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 10, 1000);        // camera view volume
}

// main (initialize glut window and register call backs)
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Art Gallery");
    initialize();

    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutTimerFunc(counter, timer, 0);

    glutMainLoop();

    return 0;
}
