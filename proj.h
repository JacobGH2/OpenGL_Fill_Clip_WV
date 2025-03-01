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

extern std::vector<vertex> polygon;
extern box clipWindow;
extern vertex finalPoint;
extern int clipFlag, completePolygonFlag, fillFlag;

void SLFill(const std::vector<vertex>);
void SHPolygonClip(std::vector<vertex> &polygon, const box window);
vertex intersect(vertex, vertex, vertex, vertex);