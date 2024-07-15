#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>

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

void generatePoints(std::vector<Point*>& points, int numPoints, std::string distributionType) {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    if (distributionType == "uniforme") {
        std::uniform_real_distribution<double> distribution(-200.0, 200.0);
        for (int i = 0; i < numPoints; ++i) {
            double x = distribution(generator);
            double y = distribution(generator);
            points.push_back(new Point(x, y));
        }
    } else if (distributionType == "normal") {
        std::normal_distribution<double> distribution(0.0, 50.0);
        for (int i = 0; i < numPoints; ++i) {
            double x = distribution(generator);
            double y = distribution(generator);
            points.push_back(new Point(x, y));
        }
    } else if (distributionType == "clusters") {
        std::uniform_real_distribution<double> clusterCenterDist(-200.0, 200.0);
        std::normal_distribution<double> clusterPointDist(0.0, 20.0);
        int numClusters = numPoints / 100;
        for (int c = 0; c < numClusters; ++c) {
            double clusterCenterX = clusterCenterDist(generator);
            double clusterCenterY = clusterCenterDist(generator);
            for (int i = 0; i < 100; ++i) {
                double x = clusterCenterX + clusterPointDist(generator);
                double y = clusterCenterY + clusterPointDist(generator);
                points.push_back(new Point(x, y));
            }
        }
    }
}

void saveResultsToCSV(const std::string& filename, const std::vector<std::string>& distributions, const std::vector<int>& numPointsList, const std::vector<double>& qtTimes, const std::vector<double>& pqtTimes) {
    std::ofstream file(filename);
    file << "distribution,num_points,quadtree_time,pointquadtree_time\n";
    for (size_t i = 0; i < distributions.size(); ++i) {
        file << distributions[i] << "," << numPointsList[i] << "," << qtTimes[i] << "," << pqtTimes[i] << "\n";
    }
    file.close();
}

int main() {
    Rectangle boundary(0, 0, 200, 200);
    std::vector<std::string> distributions = {"uniforme", "normal", "clusters"};
    int numPointsList[] = {100, 500, 1000, 5000, 10000, 20000, 50000};

    std::vector<std::string> resultsDistributions;
    std::vector<int> resultsNumPoints;
    std::vector<double> resultsQtTimes;
    std::vector<double> resultsPqtTimes;

    for (const auto& distribution : distributions) {
        for (int numPoints : numPointsList) {
            std::vector<Point*> points;
            generatePoints(points, numPoints, distribution);

            Quadtree qt(boundary, 4);
            PointQuadtree pqt(boundary);

            for (auto& point : points) {
                qt.insert(point);
                pqt.insert(point);
            }

            Rectangle range(0, 0, 50, 50);
            std::vector<Point*> found;

            auto start = std::chrono::high_resolution_clock::now();
            qt.query(&range, found);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            resultsDistributions.push_back(distribution);
            resultsNumPoints.push_back(numPoints);
            resultsQtTimes.push_back(duration.count());
            std::cout << distribution << " - Quadtree (" << numPoints << " puntos): " << found.size() << " puntos en " << duration.count() << " segundos." << std::endl;

            found.clear();
            start = std::chrono::high_resolution_clock::now();
            pqt.query(&range, found);
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            resultsPqtTimes.push_back(duration.count());
            std::cout << distribution << " - PointQuadtree (" << numPoints << " puntos): " << found.size() << " puntos en " << duration.count() << " segundos." << std::endl;

            for (auto& point : points) {
                delete point;
            }
        }
    }

    saveResultsToCSV("C:\\Users\\chiar\\Downloads\\graficas\\results.csv", resultsDistributions, resultsNumPoints, resultsQtTimes, resultsPqtTimes);

    return 0;
}

