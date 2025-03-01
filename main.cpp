#include "proj.h"

// PRIMITIVES/UTILITY -------------------------------------------------------------------

std::vector<vertex> polygon = {{100, 100}, {300, 100}, {200, 300}};
box clipWindow = {{225,350}, {425, 450}};
box viewPortWindow = {{500, 100}, {745, 500}};
box modeWindow = {{750, 550}, {800, 600}};
vertex finalPoint;
int modeFlag = POLYGON_DRAW, completePolygonFlag = 1, fillFlag = 0;

void openGLPoint(int x, int y, int size) {
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPointSize(size);
    glBegin(GL_POINTS);
        glVertex2d(x, y);
    glEnd();
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
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

vertex intersect(vertex v1, vertex v2, vertex v3, vertex v4) {
    int x1=v1.x, y1=v1.y, x2=v2.x, y2=v2.y, x3=v3.x, y3=v3.y, x4=v4.x, y4=v4.y;
    double D = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);
    double N1 = (x1*y2 - y1*x2), N2 = (x3*y4 - y3*x4);
    int x = (int) std::round((N1*(x3-x4)-(x1-x2)*N2)/D);
    int y = (int) std::round((N1*(y3-y4)-(y1-y2)*N2)/D);
    return {x, y};
}

void printPolygon() {
    for (int i = 0; i < polygon.size(); i++) {
        std::cout << "x: " << polygon[i].x << " y: " << polygon[i].y << std::endl;
    }
}   

// SCENE ELEMENTS  ----------------------------------------------------------------------

void displayBox(const box c, int stipple=0) {
    openGLLine(c.BL.x, c.TR.y, c.TR.x, c.TR.y, stipple);  
    openGLLine(c.TR.x, c.TR.y, c.TR.x, c.BL.y, stipple); 
    openGLLine(c.TR.x, c.BL.y, c.BL.x, c.BL.y, stipple); 
    openGLLine(c.BL.x, c.BL.y, c.BL.x, c.TR.y, stipple); 
}

void displayModeBox() {
    displayBox(modeWindow, 0);
    // translate box to vector<vertex> for filling
    std::vector<vertex> boxVertices = {{modeWindow.BL.x, modeWindow.BL.y}, {modeWindow.TR.x, modeWindow.BL.y}, {modeWindow.TR.x, modeWindow.TR.y}, {modeWindow.BL.x, modeWindow.TR.y}};
    int color = 0;
    switch (modeFlag) {
        case POLYGON_DRAW:
            color = J_RED;
            break;
        case CLIP_WINDOW_DRAW:
            color = J_GREEN;
            break;
        case VIEWPORT_DRAW:
            color = J_BLUE;
            break;
    }
    SLFill(boxVertices, color);
}

void displayPolygon(const std::vector<vertex> polygon) {
    for (int i = 0; i < polygon.size()-1; i++) {
        openGLLine(polygon[i].x, polygon[i].y, polygon[i+1].x, polygon[i+1].y);
    }
    if (polygon.size() >= 3 && completePolygonFlag) {
        openGLLine(polygon[polygon.size()-1].x, polygon[polygon.size()-1].y, polygon[0].x, polygon[0].y);
    }
}

// HANDLERS -----------------------------------------------------------------------------
void processMenu(int option)
{
	switch (option) {
	case 1:
        if (completePolygonFlag) {
            SHPolygonClip(polygon, clipWindow);
        }
		break;
	case 2:
        if (completePolygonFlag) {
            if (!fillFlag) fillFlag = 1;
            else fillFlag = 0;
        }
        glutPostRedisplay();
		break;
	case 3:
        if (completePolygonFlag) {
            viewportTransform(polygon, viewPortWindow);
        }
        glutPostRedisplay();
		break;
	}
}

void processMouse(int button, int state, int x, int y) {
    if (modeFlag == CLIP_WINDOW_DRAW) { // clipping mode
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            clipWindow.BL = {x, 600-y};
            clipWindow.TR = {x, 600-y};
            glutPostRedisplay();
        }
    } else if (modeFlag == POLYGON_DRAW) { // polygon drawing mode
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (completePolygonFlag == 1) {
                polygon.clear();
                completePolygonFlag = 0;
                fillFlag = 0;
                polygon.push_back({x, 600-y});
                glutPostRedisplay();
            } else {
                polygon.push_back({x, 600-y});
                glutPostRedisplay();
            }
        }
    } else if (modeFlag == VIEWPORT_DRAW) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            viewPortWindow.BL = {x, 600-y};
            viewPortWindow.TR = {x, 600-y};
            glutPostRedisplay();
        }
    }
    
}

void defineClippingWindow(int x, int y) { // active mouse
    if (modeFlag == CLIP_WINDOW_DRAW) {
        clipWindow.TR = {x, 600-y};
        glutPostRedisplay();
    } else if (modeFlag == VIEWPORT_DRAW) {
        viewPortWindow.TR = {x, 600-y};
        glutPostRedisplay();
    }
}

void dynamicPolygon(int x,  int y) { // passive mouse
    if (modeFlag == POLYGON_DRAW && !completePolygonFlag) {
        finalPoint = {x, 600-y};
    }
}

void processKeys(unsigned char key, int x, int y) {
	switch (key) {
        case 'f':
            exit(0);
            break;
        case 'r': // toggle clip window drawing/polygon drawing/viewport drawing
            switch (modeFlag) {
                case POLYGON_DRAW:
                    modeFlag = CLIP_WINDOW_DRAW;
                    break;
                case CLIP_WINDOW_DRAW:
                    modeFlag = VIEWPORT_DRAW;
                    break;
                case VIEWPORT_DRAW:
                    modeFlag = POLYGON_DRAW;
                    break;
            }
            break;
        case 'c': // complete polygon flag
            if (!completePolygonFlag) completePolygonFlag = 1;
            break;
    }
    glutPostRedisplay();
}

// MAIN FUNCTIONS -----------------------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (polygon.size() > 1) displayPolygon(polygon);

    if (!completePolygonFlag && polygon.size() > 1) {
        openGLLine(polygon[polygon.size()-1].x, polygon[polygon.size()-1].y, finalPoint.x, finalPoint.y);
    }

    if (polygon.size() == 1) openGLPoint(polygon[0].x, polygon[0].y, 5);
    
    displayBox(clipWindow, 1); // clip window

    displayModeBox(); // display mode indicator

    displayBox(viewPortWindow);

    if (fillFlag) SLFill(polygon);

    glutSwapBuffers();
}

void init() {
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
	glutAddMenuEntry("Viewport Map", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    // register callback for mouse
    glutMouseFunc(processMouse);

    // register callback for passive mouse movement
    glutMotionFunc(defineClippingWindow);

    glutPassiveMotionFunc(dynamicPolygon);

    // enter main loop
	glutMainLoop();

    return 0;
}