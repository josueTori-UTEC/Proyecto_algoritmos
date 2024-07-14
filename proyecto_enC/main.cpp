#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "Quadtree.h"
#include "PointQuadtree.h"

int main() {
    Rectangle boundary(0, 0, 200, 200);
    Quadtree qt(boundary, 4);
    PointQuadtree pqt(boundary);

    // Generar puntos aleatorios
    std::vector<Point*> points;
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(-200.0, 200.0);

    for (int i = 0; i < 1000; ++i) {
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
