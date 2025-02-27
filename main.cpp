#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <thread>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>

typedef struct {
    int x, y;
} vertex;

typedef struct {
    vertex BL, TR;
} box;

std::vector<vertex> polygon = {{400,300}, {600, 500}, {200, 500}};
box clipWindow = {{325,350}, {525, 450}};

// key handlers
void processKeys(unsigned char key, int x, int y) {
	switch (key) {
        case 'f':
            exit(0);
            break; 
    }
}

void init()
{
	// clear color buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// set the viewport as the full screen
	glViewport(0, 0, 800, 600);

	// set the projection as orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 800.0, 0.0, 600.0, 1.0, -1.0);

	// set the matrix mode back to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void openGLLine(float x1, float y1, float x2, float y2, int stipple=0) {
    if (stipple == 1) { 
        glLineStipple(1, 0x00FF); 
        glEnable(GL_LINE_STIPPLE);
    }
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();

    glDisable(GL_LINE_STIPPLE);
}

void displayPolygon(const std::vector<vertex> polygon) {
    for (int i = 0; i < polygon.size()-1; i++) {
        openGLLine(polygon[i].x, polygon[i].y, polygon[i+1].x, polygon[i+1].y);
    }
    if (polygon.size() >= 3) {
        openGLLine(polygon[polygon.size()-1].x, polygon[polygon.size()-1].y, polygon[0].x, polygon[0].y);
    }
}

void displayClipWindow(const box c) {
    openGLLine(c.BL.x, c.TR.y, c.TR.x, c.TR.y, 1);  
    openGLLine(c.TR.x, c.TR.y, c.TR.x, c.BL.y, 1); 
    openGLLine(c.TR.x, c.BL.y, c.BL.x, c.BL.y, 1); 
    openGLLine(c.BL.x, c.BL.y, c.BL.x, c.TR.y, 1); 
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    displayPolygon(polygon);

    displayClipWindow(clipWindow);


    glutSwapBuffers();
}

vertex intersect(vertex v1, vertex v2, vertex v3, vertex v4) {
    int x1=v1.x, y1=v1.y, x2=v2.x, y2=v2.y, x3=v3.x, y3=v3.y, x4=v4.x, y4=v4.y;
    double D = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    double N1 = (x1*y2 - y1*x2), N2 = (x3*y4 - y3*x4);
    int x = (int) std::round((N1*(x3-x4)-(x1-x2)*N2)/D);
    int y = (int) std::round((N1*(y3-y4)-(y1-y2)*N2)/D);
    return {x, y};
}

template <typename Compare>
std::vector<vertex> SHBorderClip(std::vector<vertex> p, Compare cmp, int clipValue, vertex bp1, vertex bp2, int yFlag) {
    std::vector<vertex> ret;

    for (int i = 0; i < p.size(); i++) {
        int in1 = 1, in2 = 1;
        int v1 = i, v2 = i+1;
        if (i == p.size()-1) {v2 = 0;}
        if (yFlag == 1) {
            if (cmp(p[v1].y, clipValue)) {in1 = 0;}
            if (cmp(p[v2].y, clipValue)) {in2 = 0;}
        } else {
            if (cmp(p[v1].x, clipValue)) {in1 = 0;}
            if (cmp(p[v2].x, clipValue)) {in2 = 0;}
        }
        if (!in1 && in2) { // v1', v2
            ret.push_back(intersect(p[v1], p[v2], bp1, bp2));
            ret.push_back(p[v2]);
        }
        if (in1 && !in2) { // v1'
            ret.push_back(intersect(p[v1], p[v2], bp1, bp2));
        }
        if (in1 && in2) {
            ret.push_back(p[v2]);
        }
    }

    return ret;
}

void SHPolygonClip(std::vector<vertex> &polygon, const box w) {
    std::vector<vertex> p = polygon;
    // left
    std::vector<vertex> pLeft = SHBorderClip(p, std::less<int>(), w.BL.x, w.BL, {w.BL.x, w.TR.y}, 0);
    // right
    std::vector<vertex> pRight = SHBorderClip(pLeft, std::greater<int>(), w.TR.x, {w.TR.x, w.BL.y}, w.TR, 0);
    // top
    std::vector<vertex> pTop = SHBorderClip(pRight, std::greater<int>(), w.TR.y, {w.BL.x, w.TR.y}, w.TR, 1);
    // bottom
    std::vector<vertex> pBottom = SHBorderClip(pTop, std::less<int>(), w.BL.y, w.BL, {w.TR.x, w.BL.y}, 1);

    polygon = pBottom;
    glutPostRedisplay();
}

// menu handler
void processMenu(int option)
{
	switch (option) {
	case 1:
        SHPolygonClip(polygon, clipWindow);
		break;
	case 2:
        // fill polygon
		break;
	case 3:
        // perform WV transform
		break;
	}
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
	// create a window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Clipping, Filling, and WV");

	// init settings
	init();
	glutDisplayFunc(display);

    // register a callback for keyboard events
	glutKeyboardFunc(processKeys);

    // create a menu and add entries
	int menu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Clip", 1);
	glutAddMenuEntry("Fill", 2);
	glutAddMenuEntry("Viewport", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    // register callback for mouse
    //glutMouseFunc(processMouse);

    // register callback for passive mouse movement
    //glutPassiveMotionFunc(dynamicDraw);

    // enter main loop
	glutMainLoop();

    return 0;
}