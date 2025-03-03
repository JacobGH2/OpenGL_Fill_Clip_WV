#include "proj.h"

void viewportTransform(std::vector<vertex> wc, std::vector<vertex> &p, const box viewportW, const box worldW) {
    box VW = formatSelectingWindow(viewportW);
    box WW = formatSelectingWindow(worldW);

    // always mapping from entire window to viewport
    double xScale = ((VW.TR.x - VW.BL.x)/(double)(WW.TR.x - WW.BL.x));
    double yScale = ((VW.TR.y - VW.BL.y)/(double)(WW.TR.y - WW.BL.y));

    p.clear();

    for (int i = 0; i < wc.size(); i++) {
        int x = std::round((wc[i].x - (double)WW.BL.x) * xScale + VW.BL.x);
        int y = std::round((wc[i].y - (double)WW.BL.y) * yScale + VW.BL.y);
        p.push_back({x, y});
    }
}