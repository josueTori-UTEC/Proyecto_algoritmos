#ifndef POINTQUADTREE_H
#define POINTQUADTREE_H

#include "Rectangle.h"
#include <vector>

class PointQuadtree {
private:
    Point* point;
    Rectangle boundary;
    PointQuadtree* nw;
    PointQuadtree* ne;
    PointQuadtree* sw;
    PointQuadtree* se;

public:
    PointQuadtree(Rectangle _boundary): boundary(_boundary), point(nullptr), nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}

    ~PointQuadtree(){
        delete nw;
        delete ne;
        delete sw;
        delete se;
    }

    bool insert(Point* p){
        if (!boundary.contains(p)) return false;

        if (point == nullptr) {
            point = p;
            return true;
        }

        if (p->x < point->x) {
            if (p->y < point->y) {
                if (!sw) {
                    Rectangle rect(boundary.x - boundary.width / 2, boundary.y - boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    sw = new PointQuadtree(rect);
                }
                return sw->insert(p);
            } else {
                if (!nw) {
                    Rectangle rect(boundary.x - boundary.width / 2, boundary.y + boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    nw = new PointQuadtree(rect);
                }
                return nw->insert(p);
            }
        } else {
            if (p->y < point->y) {
                if (!se) {
                    Rectangle rect(boundary.x + boundary.width / 2, boundary.y - boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    se = new PointQuadtree(rect);
                }
                return se->insert(p);
            } else {
                if (!ne) {
                    Rectangle rect(boundary.x + boundary.width / 2, boundary.y + boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    ne = new PointQuadtree(rect);
                }
                return ne->insert(p);
            }
        }
    }


    void query(Rectangle* range, std::vector<Point*>& found) const {
        if (!boundary.intersects(range)) return;

        if (point && range->contains(point)) {
            found.push_back(point);
        }

        if (nw) nw->query(range, found);
        if (ne) ne->query(range, found);
        if (sw) sw->query(range, found);
        if (se) se->query(range, found);
    }


    bool remove(Point* p){
        if (!boundary.contains(p)) return false;

        if (point && point == p) {
            point = nullptr;
            return true;
        }

        if (nw && nw->remove(p)) return true;
        if (ne && ne->remove(p)) return true;
        if (sw && sw->remove(p)) return true;
        if (se && se->remove(p)) return true;

        return false;
    }
};

#endif 
