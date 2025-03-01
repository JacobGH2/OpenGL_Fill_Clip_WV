#include "proj.h"

void viewportTransform(std::vector<vertex> &p, const box window) {
    box w = formatSelectingWindow(window);

    // always mapping from entire window to viewport
    double xScale = ((w.TR.x - w.BL.x)/800.0);
    double yScale = ((w.TR.y - w.BL.y)/600.0);

    for (int i = 0; i < p.size(); i++) {
        p[i].x = std::round((p[i].x - 0) * xScale + w.BL.x);
        p[i].y = std::round((p[i].y - 0) * yScale + w.BL.y);
    }
    glutPostRedisplay();
}