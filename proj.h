#ifndef PROJ
#define PROJ
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
#endif

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

const int POLYGON_DRAW = 0, CLIP_WINDOW_DRAW = 1, VIEWPORT_DRAW = 2;
const int J_WHITE = 0, J_RED = 1, J_GREEN = 2, J_BLUE = 3;
void SLFill(const std::vector<vertex>, int color = J_WHITE);
void SHPolygonClip(std::vector<vertex> &polygon, const box window);
vertex intersect(vertex, vertex, vertex, vertex);
void viewportTransform(std::vector<vertex> wc, std::vector<vertex> &p, const box viewportW, const box worldW);
box formatSelectingWindow(const box);
void printPolygons(); // debug