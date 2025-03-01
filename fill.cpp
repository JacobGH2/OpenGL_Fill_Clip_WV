#include "proj.h"

bool myComp(edgeEntry i, edgeEntry j) { return (i.xMin < j.xMin); }

void SLFill(std::vector<vertex> p, int color) {
    glEnable(GL_COLOR_MATERIAL);
    // set color
    switch (color) {
        case J_RED:
            glColor3ub(255, 0, 0);
            break;
        case J_GREEN:
            glColor3ub(0, 255, 0);
            break;
        case J_BLUE:
            glColor3ub(0, 0, 255);
            break;
    }

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
            glBegin(GL_LINES);
                glVertex2d(fillPtsX[j], i);
                glVertex2d(fillPtsX[j+1], i);
            glEnd();
        }
        // ----------------------------------------------------------------
    }
    glColor3ub(255, 255, 255);
    glDisable(GL_COLOR_MATERIAL);

    glutPostRedisplay();

}