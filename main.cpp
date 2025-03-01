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
#include <tuple>
#include <algorithm>

typedef struct {
    int x, y;
} vertex;

typedef struct {
    vertex v1, v2;
} edge;

typedef struct {
    vertex BL, TR;
} box;

typedef struct {
    edge e;
    int yMax, xMin;
} edgeEntry;

std::vector<vertex> polygon = {{538, 190},
{499, 285},
{420, 336},
{277, 337},
{133, 303},
{139, 154},
{285, 110},
{369, 203},
{481, 92}};
box clipWindow = {{325,350}, {525, 450}};
vertex finalPoint;
int clipFlag = 0, completePolygonFlag = 1, fillFlag = 0;

void init();
void SLFill(const std::vector<vertex>);

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
    if (polygon.size() >= 3 && completePolygonFlag) {
        openGLLine(polygon[polygon.size()-1].x, polygon[polygon.size()-1].y, polygon[0].x, polygon[0].y);
    }
}

void displayClipWindow(const box c) {
    openGLLine(c.BL.x, c.TR.y, c.TR.x, c.TR.y, 1);  
    openGLLine(c.TR.x, c.TR.y, c.TR.x, c.BL.y, 1); 
    openGLLine(c.TR.x, c.BL.y, c.BL.x, c.BL.y, 1); 
    openGLLine(c.BL.x, c.BL.y, c.BL.x, c.TR.y, 1); 
}

void openGLFirstPoint(int x, int y, int size) {
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPointSize(size);
    glBegin(GL_POINTS);
        glVertex2d(x, y);
    glEnd();
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (polygon.size() > 1) {
        displayPolygon(polygon);
    }
    if (!completePolygonFlag && polygon.size() > 1) {
        openGLLine(polygon[polygon.size()-1].x, polygon[polygon.size()-1].y, finalPoint.x, finalPoint.y);
    }
    if (polygon.size() == 1) openGLFirstPoint(polygon[0].x, polygon[0].y, 5);
    
    displayClipWindow(clipWindow);

    if (fillFlag) {
       SLFill(polygon);
    }

    //openGLLine(0, 204, 800, 204);

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

box formatClipWindow(const box b) {
    if (b.TR.x < b.BL.x && b.TR.y > b.BL.y) { // Q2
        return {{b.TR.x, b.BL.y}, {b.BL.x, b.TR.y}};
    }
    if (b.TR.x < b.BL.x && b.TR.y < b.BL.y) { // Q3
        return {{b.TR.x, b.TR.y}, {b.BL.x, b.BL.y}};
    }
    if (b.BL.x < b.TR.x && b.TR.y < b.BL.y) { // Q4
        return {{b.BL.x, b.TR.y}, {b.TR.x, b.BL.y}};
    }
    return b;
}

void SHPolygonClip(std::vector<vertex> &polygon, const box window) {
    box w = formatClipWindow(window);

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
        if (completePolygonFlag == 1){
            SHPolygonClip(polygon, clipWindow);
        }
		break;
	case 2:
        if (completePolygonFlag) {fillFlag = 1;}
        glutPostRedisplay();
		break;
	case 3:
        // perform WV transform
		break;
	}
}

void processMouse(int button, int state, int x, int y) {
    if (clipFlag == 1) { // clipping mode
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            clipWindow.BL = {x, 600-y};
            clipWindow.TR = {x, 600-y};
            glutPostRedisplay();
        }
    } else if (clipFlag == 0) { // polygon drawing mode
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
    }
    
}

void defineClippingWindow(int x, int y) { // active mouse
    if (clipFlag == 1) {
        clipWindow.TR = {x, 600-y};
        glutPostRedisplay();
    }
}

void dynamicPolygon(int x,  int y) { // passive mouse
    if (!clipFlag && !completePolygonFlag) {
        finalPoint = {x, 600-y};
    }
}

// key handlers
void processKeys(unsigned char key, int x, int y) {
	switch (key) {
        case 'f':
            exit(0);
            break;
        case 'r': // toggle clipping/polygon drawing
            if (clipFlag) clipFlag = 0;
            else clipFlag = 1;
            glutPostRedisplay();
            break;
        case 'c': // complete polygon flag
            if (!completePolygonFlag) completePolygonFlag = 1;
            glutPostRedisplay();
            break;
    }
}

bool myComp(edgeEntry i, edgeEntry j) { return (i.xMin < j.xMin); }

void SLFill(std::vector<vertex> p) {

    // determine ET size (maximum Ymin) and initialize empty ET
    int maxY = -1;
    for (int i = 0; i < p.size(); i++) {
        maxY = std::max(maxY, p[i].y);
    }
    std::vector<std::tuple<int, std::vector<edgeEntry>>> ET;
    for (int i = 0; i < maxY; i++) {
        std::vector<edgeEntry> t;
        ET.emplace_back(0, t);
    }
    
    for (int i = 0; i < p.size(); i++) {  // add edges to ET in correct bucket (index == yMin)
        int last = i+1; // index wrap-around
        if (i == p.size()-1) last = 0; 

        if (p[i].y == p[last].y) { // don't add horizontal line
            continue;
        }

        edge modEdge = {{p[i].x, p[i].y}, {p[last].x, p[last].y}}; // add edge and supporting values
        int minX = std::min(modEdge.v1.x, modEdge.v2.x);
        int maxY = std::max(modEdge.v1.y, modEdge.v2.y);
        int minY = std::min(modEdge.v1.y, modEdge.v2.y);
        edgeEntry t = {modEdge, maxY, minX};
        std::get<0>(ET[minY]) = 1;
        std::get<1>(ET[minY]).push_back(t);
        
    }

    // sort all lists by xMin
    for (int i = 0; i < ET.size(); i++) {
        if (std::get<0>(ET[i]) == 1) {
            std::sort(std::get<1>(ET[i]).begin(), std::get<1>(ET[i]).end(), myComp);
        }
    }

    std::vector<edgeEntry> activeET; // active edge table

    // main loop for all y=[0..yMax] ----------------------------------------------------
    for (int i = 0; i < ET.size(); i++) { 

        for (int j = 0; j < activeET.size(); j++) { // remove from AET if needed
            if (activeET[j].yMax < i) {
                activeET.erase(activeET.begin() + j);
                j -= 1; // avoid going too far
            }
        }
        if (std::get<0>(ET[i]) == 1) { // add to AET if current elev if non-empty
            for (int k = 0; k < std::get<1>(ET[i]).size(); k++) { // add edges with y starting at i 
                activeET.push_back(std::get<1>(ET[i])[k]);
            }
        }

        // Calculate Intersections and Fill -------------------------------

        std::vector<int> fillPtsX; // only have to store x, all have same y
        for (int j = 0; j < activeET.size(); j++) { // calculate intersections
            vertex inter = intersect({0, i}, {800, i}, activeET[j].e.v1, activeET[j].e.v2);
            fillPtsX.push_back(inter.x);
        }

        // AET indices correspond to fillPtsX indices

        int oddInt = 0;
        if (fillPtsX.size() % 2 != 0) oddInt = 1;

        if (oddInt) {
            // erase duplicate int. point if it corresponds to edge whose top vertex has current y-value
            int k = 0;
            while (k < fillPtsX.size()) {
                if (std::count(fillPtsX.begin(), fillPtsX.end(), fillPtsX[k]) > 1) {
                    if (std::max(activeET[k].e.v1.y, activeET[k].e.v2.y) == i) {
                        fillPtsX.erase(fillPtsX.begin() + k);
                        continue;
                    } else {
                        k++;
                        continue;
                    }
                }
                k++;
            }
        }     

        std::sort(fillPtsX.begin(), fillPtsX.end()); // sort by x int point

        for (int j = 0; j < fillPtsX.size(); j += 2) {
            glColor3d(233, 204, 153);
            glBegin(GL_LINES);
                glVertex2d(fillPtsX[j], i);
                glVertex2d(fillPtsX[j+1], i);
            glEnd();
        }
        // ----------------------------------------------------------------
    }

    glutPostRedisplay();

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
    glutMouseFunc(processMouse);

    // register callback for passive mouse movement
    glutMotionFunc(defineClippingWindow);

    glutPassiveMotionFunc(dynamicPolygon);

    // enter main loop
	glutMainLoop();

    return 0;
}