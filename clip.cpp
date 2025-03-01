#include "proj.h"

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