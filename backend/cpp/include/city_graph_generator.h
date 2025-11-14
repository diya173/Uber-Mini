#ifndef CITY_GRAPH_GENERATOR_H
#define CITY_GRAPH_GENERATOR_H

#include "graph.h"
#include "driver_manager.h"
#include <vector>
#include <cmath>
#include <random>

namespace RideSharing {

struct CityData {
    Graph* graph;
    std::vector<Driver> drivers;

    ~CityData() {
        delete graph;
    }
};

struct NodeData {
    int id;
    double lat;
    double lon;
    int sector;
};

class CityGraphGenerator {
public:
    /**
     * Generate a realistic complex city graph
     * @param numNodes Number of nodes to generate (default: 50)
     * @return CityData containing graph and drivers
     */
    static CityData* generateCityGraph(int numNodes = 50);

private:
    static void createHighways(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes);
    static void createArterialRoads(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes);
    static void createLocalStreets(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes);
    static void createRingRoads(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes);
    static void createShortcuts(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes);
    static void ensureConnectivity(Graph* graph);

    static double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    static double toRad(double degrees);
    static std::string getOrdinal(int n);

    static std::vector<Driver> generateDrivers();
    static std::vector<std::string> getAllLocationNames();
};

} // namespace RideSharing

#endif // CITY_GRAPH_GENERATOR_H
