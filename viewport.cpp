#include "proj.h"

void viewportTransform(std::vector<vertex> wc, std::vector<vertex> &p, const box window) {
    box w = formatSelectingWindow(window);

    // always mapping from entire window to viewport
    double xScale = ((w.TR.x - w.BL.x)/800.0);
    double yScale = ((w.TR.y - w.BL.y)/600.0);

    p.clear();

    for (int i = 0; i < wc.size(); i++) {
        int x = std::round((wc[i].x - 0) * xScale + w.BL.x);
        int y = std::round((wc[i].y - 0) * yScale + w.BL.y);
        p.push_back({x, y});
    }
}