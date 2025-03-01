std::vector<edge *> edges; // for point modification
    for (int i = 0; i < ET.size(); i++) { // pushing pointers into mod vector
        std::cout << "outer" << std::endl;
        if (std::get<0>(ET[i]) == 1) { // valid
            std::vector<edgeEntry> *curr = &(std::get<1>(ET[i]));
            for (int j = 0; j << std::get<1>(ET[i]).size(); j++) { // add pointers to all in bucket
                std::cout << "curr: " << curr->at(j).e.v1.x << std::endl;
                edges.push_back(&(curr->at(j).e));
            }
        }
    }


    int i = 0;
    while (i < edges.size()) { // edge modification
        int iP = i+1;
        if (i == edges.size()-1) iP = 0;
        
        int iMin = std::min(edges[i]->v1.y, edges[i]->v2.y);
        int iMax = std::max(edges[i]->v1.y, edges[i]->v2.y);
        int ipMax = std::max(edges[iP]->v1.y, edges[iP]->v2.y);

        std::cout << "iMin: " << iMin << " iMax: " << iMax << " ipMax: " << ipMax << std::endl;
        if (iMin < iMax && iMax < ipMax) { // monotonous increase
            edges[i]->v2.y -= 1;
            i++;
            continue;
        }

        int iMax2 = std::max(edges[i]->v1.y, edges[i]->v2.y);
        int iMin2 = std::min(edges[i]->v1.y, edges[i]->v2.y);
        int ipMin = std::min(edges[iP]->v1.y, edges[iP]->v2.y);

        if (iMax > iMin2 && iMin2 > ipMin) { // monotonous decrease
            if (edges[iP]->v1.y > edges[iP]->v2.y) edges[iP]->v1.y -= 1;
            else edges[iP]->v2.y -= 1;
            std::cout << "monotonous decrease" << std::endl;
            i++;
            continue;
        }

        // otherwise, no modification needed

        i++;
    }


    int detectMonotonous(edge curr, edge next) {
    
        int iMin = std::min(curr.v1.y, curr.v2.y);
        int iMax = std::max(curr.v1.y, curr.v2.y);
        int ipMax = std::max(next.v1.y, next.v2.y);
    
        if (iMin < iMax && iMax < ipMax) { // mon. increase
            return 1; // decrement current y2
        }
    
        int iMax2 = std::max(curr.v1.y, curr.v2.y);
        int iMin2 = std::min(curr.v1.y, curr.v2.y);
        int ipMin = std::min(next.v1.y, next.v2.y);
    
        if (iMax2 > iMin2 && iMin2 > ipMin) { // mon. decrease
            return 2; // decrement next y1
        }   
    
        return 0;
    }


 // debug print
    /*
    for (int i = 0; i < ET.size(); i++) {
        if (std::get<0>(ET[i]) == 1) {
            std::cout << "bucket" << std::endl;
            std::vector<edgeEntry> curr = std::get<1>(ET[i]);
            for (int j = 0; j < curr.size(); j++) {
                std::cout << "x1: " << curr[j].e.v1.x << " y1: " << curr[j].e.v1.y << " x2: " << curr[j].e.v2.x << " y2: " << curr[j].e.v2.y << std::endl;
            }
        }
    } */