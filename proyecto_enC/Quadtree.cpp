#include "Quadtree.h"

Quadtree::Quadtree(Rectangle _boundary, int _capacity)
    : boundary(_boundary), capacity(_capacity),
      northeast(nullptr), northwest(nullptr),
      southeast(nullptr), southwest(nullptr) {}

Quadtree::~Quadtree() {
    delete northeast;
    delete northwest;
    delete southeast;
    delete southwest;
}

void Quadtree::subdivide() {
    double x = boundary.x;
    double y = boundary.y;
    double w = boundary.width / 2;
    double h = boundary.height / 2;

    Rectangle ne(x + w, y - h, w, h);
    Rectangle nw(x - w, y - h, w, h);
    Rectangle se(x + w, y + h, w, h);
    Rectangle sw(x - w, y + h, w, h);

    northeast = new Quadtree(ne, capacity);
    northwest = new Quadtree(nw, capacity);
    southeast = new Quadtree(se, capacity);
    southwest = new Quadtree(sw, capacity);

    divided = true;
}

bool Quadtree::insert(Point* point) {
    if (!boundary.contains(point)) {
        return false;
    }

    if (points.size() < capacity) {
        points.push_back(point);
        return true;
    } else {
        if (!divided) {
            subdivide();
        }

        if (northeast->insert(point)) return true;
        if (northwest->insert(point)) return true;
        if (southeast->insert(point)) return true;
        if (southwest->insert(point)) return true;
    }

    return false;
}

void Quadtree::query(Rectangle* range, std::vector<Point*>& found) const {
    if (!boundary.intersects(range)) {
        return;
    } else {
        for (auto& point : points) {
            if (range->contains(point)) {
                found.push_back(point);
            }
        }
        if (divided) {
            northeast->query(range, found);
            northwest->query(range, found);
            southeast->query(range, found);
            southwest->query(range, found);
        }
    }
}

bool Quadtree::remove(Point* point) {
    if (!boundary.contains(point)) {
        return false;
    }

    for (auto it = points.begin(); it != points.end(); ++it) {
        if (*it == point) {
            points.erase(it);
            return true;
        }
    }

    if (divided) {
        if (northeast->remove(point)) return true;
        if (northwest->remove(point)) return true;
        if (southeast->remove(point)) return true;
        if (southwest->remove(point)) return true;
    }

    return false;
}
