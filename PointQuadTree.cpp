#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <memory>
#include <algorithm>

struct Point {
    double x, y;
    Point(double _x, double _y) : x(_x), y(_y) {}
};

struct Rectangle {
    double x, y, width, height;
    Rectangle(double _x, double _y, double _width, double _height)
            : x(_x), y(_y), width(_width), height(_height) {}

    bool contains(const Point* point) const {
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

class PointQuadtree {
private:
    std::unique_ptr<Point> point;
    Rectangle boundary;
    std::unique_ptr<PointQuadtree> nw;
    std::unique_ptr<PointQuadtree> ne;
    std::unique_ptr<PointQuadtree> sw;
    std::unique_ptr<PointQuadtree> se;

public:
    PointQuadtree(Rectangle _boundary)
            : boundary(_boundary) {}

    bool insert(std::unique_ptr<Point> p) {
        if (!boundary.contains(p.get())) return false;

        if (!point) {
            point = std::move(p);
            return true;
        }

        if (p->x < point->x) {
            if (p->y < point->y) {
                if (!sw) {
                    Rectangle rect(boundary.x - boundary.width / 2, boundary.y - boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    sw = std::make_unique<PointQuadtree>(rect);
                }
                return sw->insert(std::move(p));
            } else {
                if (!nw) {
                    Rectangle rect(boundary.x - boundary.width / 2, boundary.y + boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    nw = std::make_unique<PointQuadtree>(rect);
                }
                return nw->insert(std::move(p));
            }
        } else {
            if (p->y < point->y) {
                if (!se) {
                    Rectangle rect(boundary.x + boundary.width / 2, boundary.y - boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    se = std::make_unique<PointQuadtree>(rect);
                }
                return se->insert(std::move(p));
            } else {
                if (!ne) {
                    Rectangle rect(boundary.x + boundary.width / 2, boundary.y + boundary.height / 2, boundary.width / 2, boundary.height / 2);
                    ne = std::make_unique<PointQuadtree>(rect);
                }
                return ne->insert(std::move(p));
            }
        }
    }

    void query(const Rectangle* range, std::vector<const Point*>& found) const {
        if (!boundary.intersects(range)) return;

        if (point && range->contains(point.get())) {
            found.push_back(point.get());
        }

        if (nw) nw->query(range, found);
        if (ne) ne->query(range, found);
        if (sw) sw->query(range, found);
        if (se) se->query(range, found);
    }

    bool remove(const Point* p) {
        if (!boundary.contains(p)) return false;

        if (point && point.get() == p) {
            point.reset();
            return true;
        }

        if (nw && nw->remove(p)) return true;
        if (ne && ne->remove(p)) return true;
        if (sw && sw->remove(p)) return true;
        if (se && se->remove(p)) return true;

        return false;
    }

    void saveToCSV(std::ofstream& file) const {
        if (point) {
            file << boundary.x << "," << boundary.y << "," << boundary.width << "," << boundary.height << "," << point->x << "," << point->y << "\n";
        } else {
            file << boundary.x << "," << boundary.y << "," << boundary.width << "," << boundary.height << ",,\n";
        }

        if (nw) nw->saveToCSV(file);
        if (ne) ne->saveToCSV(file);
        if (sw) sw->saveToCSV(file);
        if (se) se->saveToCSV(file);
    }
};

void generatePoints(std::vector<std::unique_ptr<Point>>& points, int numPoints, const std::string& distributionType) {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    if (distributionType == "uniforme") {
        std::uniform_real_distribution<double> distribution(-200.0, 200.0);
        for (int i = 0; i < numPoints; ++i) {
            double x = distribution(generator);
            double y = distribution(generator);
            points.push_back(std::make_unique<Point>(x, y));
        }
    } else if (distributionType == "normal") {
        std::normal_distribution<double> distribution(0.0, 50.0);
        for (int i = 0; i < numPoints; ++i) {
            double x = distribution(generator);
            double y = distribution(generator);
            points.push_back(std::make_unique<Point>(x, y));
        }
    } else if (distributionType == "clusters") {
        std::uniform_real_distribution<double> clusterCenterDist(-200.0, 200.0);
        std::normal_distribution<double> clusterPointDist(0.0, 20.0);
        int numClusters = numPoints / 10;
        for (int c = 0; c < numClusters; ++c) {
            double clusterCenterX = clusterCenterDist(generator);
            double clusterCenterY = clusterCenterDist(generator);
            for (int i = 0; i < 10; ++i) {
                double x = clusterCenterX + clusterPointDist(generator);
                double y = clusterCenterY + clusterPointDist(generator);
                points.push_back(std::make_unique<Point>(x, y));
            }
        }
    }
}

void saveQuadtreeToCSV(const std::string& filename, const PointQuadtree& qt) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return;
    }
    file << "boundary_x,boundary_y,boundary_width,boundary_height,point_x,point_y\n";
    qt.saveToCSV(file);
    file.close();
    std::cout << "Archivo CSV guardado correctamente." << std::endl;
}

int main() {
    Rectangle boundary(0, 0, 200, 200);
    PointQuadtree pqt(boundary);

    std::vector<std::unique_ptr<Point>> points;
    generatePoints(points, 1000, "uniforme");  // Incrementar el número de puntos para una mejor visualización
    std::cout << "Generados " << points.size() << " puntos." << std::endl;

    for (auto& point : points) {
        pqt.insert(std::move(point));
    }
    std::cout << "Puntos insertados en el PointQuadtree." << std::endl;

    std::string filenameCSV = "C:\\Users\\chiar\\Downloads\\graficas\\pointquadtree_structure.csv";
    saveQuadtreeToCSV(filenameCSV, pqt);
    std::cout << "Archivo guardado en: " << filenameCSV << std::endl;

    // Ejemplo de consulta
    Rectangle range(0, 0, 50, 50);
    std::vector<const Point*> found;
    pqt.query(&range, found);
    std::cout << "Puntos encontrados en el rango: " << found.size() << std::endl;

    // Ejemplo de eliminación
    if (!points.empty()) {
        const Point* pointToRemove = found[0];
        bool removed = pqt.remove(pointToRemove);
        std::cout << "Punto " << (removed ? "eliminado" : "no encontrado") << std::endl;
    }

    return 0;
}
