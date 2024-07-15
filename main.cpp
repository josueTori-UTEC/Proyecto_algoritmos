#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include "Quadtree.h"
#include "PointQuadtree.h"


void generatePoints(std::vector<Point*>& points, int numPoints, const std::string& distributionType) {
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

    saveResultsToCSV("results.csv", resultsDistributions, resultsNumPoints, resultsQtTimes, resultsPqtTimes);

    return 0;
}
