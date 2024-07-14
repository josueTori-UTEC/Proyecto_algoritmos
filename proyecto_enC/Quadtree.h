#ifndef QUADTREE_H
#define QUADTREE_H

#include "Rectangle.h"
#include <vector>

class Quadtree {
private:
    Rectangle boundary;
    int capacity;
    std::vector<Point*> points;
    bool divided = false;

    Quadtree *northeast;
    Quadtree *northwest;
    Quadtree *southeast;
    Quadtree *southwest;

public:
    Quadtree(Rectangle _boundary, int _capacity);
    ~Quadtree();

    void subdivide();
    bool insert(Point* point);
    void query(Rectangle* range, std::vector<Point*>& found) const;
    bool remove(Point* point);
};

#endif // QUADTREE_H
