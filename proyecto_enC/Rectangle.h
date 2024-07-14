#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

struct Rectangle {
    double x, y, width, height;
    Rectangle(double _x, double _y, double _width, double _height)
        : x(_x), y(_y), width(_width), height(_height) {}

    bool contains(Point* point) const {
        return (point->x >= x - width &&
                point->x <= x + width &&
                point->y >= y - height &&
                point->y <= y + height);
    }

    bool intersects(const Rectangle* range) const {
        return !(range->x - range->width > x + width ||
                 range->x + range->width < x - width ||
                 range->y - range->height > y + height ||
                 range->y + range->height < y - height);
    }
};

#endif // RECTANGLE_H
