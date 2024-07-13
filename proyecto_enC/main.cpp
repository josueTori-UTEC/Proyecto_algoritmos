#include <iostream>
#include <vector>
#include <chrono>
#include <random>

struct Point {
    double x, y;
    Point(double _x, double _y) : x(_x), y(_y) {}
};

struct Rectangle {
    double x, y, width, height;
    Rectangle(double _x, double _y, double _width, double _height) 
        : x(_x), y(_y), width(_width), height(_height) {}

    bool contains(Point* point) {
        return (point->x >= x - width &&
                point->x <= x + width &&
                point->y >= y - height &&
                point->y <= y + height);
    }

    bool intersects(Rectangle* range) {
        return !(range->x - range->width > x + width ||
                 range->x + range->width < x - width ||
                 range->y - range->height > y + height ||
                 range->y + range->height < y - height);
    }
};



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
    Quadtree(Rectangle _boundary, int _capacity) 
        : boundary(_boundary), capacity(_capacity),
          northeast(nullptr), northwest(nullptr),
          southeast(nullptr), southwest(nullptr) {}

    ~Quadtree() {
        delete northeast;
        delete northwest;
        delete southeast;
        delete southwest;
    }

    void subdivide() {
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

    bool insert(Point* point) {
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

    void query(Rectangle* range, std::vector<Point*>& found) {
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

    bool remove(Point* point) {
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
};

class PointQuadtree {
private:
    Point* point;
    Rectangle boundary;
    PointQuadtree* nw;
    PointQuadtree* ne;
    PointQuadtree* sw;
    PointQuadtree* se;

public:
    PointQuadtree(Rectangle _boundary) 
        : boundary(_boundary), point(nullptr), nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}

    ~PointQuadtree() {
        delete nw;
        delete ne;
        delete sw;
        delete se;
    }

    bool insert(Point* p) {
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

    void query(Rectangle* range, std::vector<Point*>& found) {
        if (!boundary.intersects(range)) return;

        if (point && range->contains(point)) {
            found.push_back(point);
        }

        if (nw) nw->query(range, found);
        if (ne) ne->query(range, found);
        if (sw) sw->query(range, found);
        if (se) se->query(range, found);
    }

    bool remove(Point* p) {
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

int main() {
    Rectangle boundary(0, 0, 200, 200);
    Quadtree qt(boundary, 4);
    PointQuadtree pqt(boundary);

    // Generar puntos aleatorios
    std::vector<Point*> points;
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(-200.0, 200.0);

    for (int i = 0; i < 1000; ++i) {// aca modificar para hacer los graficos 
        double x = distribution(generator);
        double y = distribution(generator);
        Point* p = new Point(x, y);
        points.push_back(p);
        qt.insert(p);
        pqt.insert(p);
    }

    // Realizar búsqueda y medir el tiempo en Quadtree
    Rectangle range(0, 0, 50, 50);
    std::vector<Point*> found;

    auto start = std::chrono::high_resolution_clock::now();
    qt.query(&range, found);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Quadtree encontrados: " << found.size() << " puntos en " << duration.count() << " segundos." << std::endl;

    // Realizar búsqueda y medir el tiempo en Point Quadtree
    found.clear();
    start = std::chrono::high_resolution_clock::now();
    pqt.query(&range, found);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "PointQuadtree encontrados: " << found.size() << " puntos en " << duration.count() << " segundos." << std::endl;

    // Limpiar memoria
    for (auto& p : points) {
        delete p;
    }

    return 0;
}
