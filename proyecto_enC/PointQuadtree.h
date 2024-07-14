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
    PointQuadtree(Rectangle _boundary);
    ~PointQuadtree();

    bool insert(Point* p);
    void query(Rectangle* range, std::vector<Point*>& found) const;
    bool remove(Point* p);
};

#endif // POINTQUADTREE_H
