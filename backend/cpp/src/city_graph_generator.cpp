#include "include/city_graph_generator.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace RideSharing {

// Random number generator
static std::random_device rd;
static std::mt19937 gen(rd());

std::vector<std::string> CityGraphGenerator::getAllLocationNames() {
    std::vector<std::string> allNames = {
        // Downtown
        "City Hall", "Financial District", "Business Center", "Central Station", "City Square",
        // Residential
        "Maple Grove", "Oak Hills", "Pine Valley", "Riverside", "Sunset Heights", "Harbor View",
        // Commercial
        "Shopping Mall", "Market Place", "Plaza", "Trade Center", "Outlet Mall",
        // Education
        "University", "College", "High School", "Elementary School", "Library",
        // Healthcare
        "General Hospital", "Medical Center", "Clinic", "Emergency Care",
        // Transport
        "Airport", "Train Station", "Bus Terminal", "Metro Hub", "Ferry Terminal",
        // Recreation
        "Central Park", "Sports Stadium", "Theater", "Museum", "Convention Center", "Zoo",
        // Industrial
        "Industrial Park", "Warehouse District", "Factory Zone", "Tech Park",
        // Misc
        "Hotel District", "Restaurant Row", "Gym", "Police Station", "Fire Station", "Post Office"
    };
    return allNames;
}

std::vector<Driver> CityGraphGenerator::generateDrivers() {
    std::vector<Driver> drivers;

    Driver d1("D001", "Rajesh Kumar", 0, "Sedan", 4.8);
    d1.isAvailable = true;
    d1.completedRides = 234;
    drivers.push_back(d1);

    Driver d2("D002", "Priya Sharma", 8, "SUV", 4.9);
    d2.isAvailable = true;
    d2.completedRides = 412;
    drivers.push_back(d2);

    Driver d3("D003", "Amit Patel", 15, "Sedan", 4.7);
    d3.isAvailable = true;
    d3.completedRides = 189;
    drivers.push_back(d3);

    Driver d4("D004", "Sneha Reddy", 22, "Compact", 4.6);
    d4.isAvailable = true;
    d4.completedRides = 156;
    drivers.push_back(d4);

    Driver d5("D005", "Vikram Singh", 30, "SUV", 4.9);
    d5.isAvailable = true;
    d5.completedRides = 567;
    drivers.push_back(d5);

    Driver d6("D006", "Anjali Verma", 35, "Sedan", 4.8);
    d6.isAvailable = false;
    d6.completedRides = 301;
    drivers.push_back(d6);

    Driver d7("D007", "Arjun Mehta", 42, "Luxury", 5.0);
    d7.isAvailable = true;
    d7.completedRides = 89;
    drivers.push_back(d7);

    Driver d8("D008", "Kavya Iyer", 48, "Sedan", 4.7);
    d8.isAvailable = true;
    d8.completedRides = 267;
    drivers.push_back(d8);

    Driver d9("D009", "Rahul Gupta", 12, "SUV", 4.9);
    d9.isAvailable = true;
    d9.completedRides = 345;
    drivers.push_back(d9);

    Driver d10("D010", "Deepika Nair", 25, "Compact", 4.8);
    d10.isAvailable = true;
    d10.completedRides = 278;
    drivers.push_back(d10);

    Driver d11("D011", "Sanjay Desai", 38, "Sedan", 4.6);
    d11.isAvailable = true;
    d11.completedRides = 198;
    drivers.push_back(d11);

    Driver d12("D012", "Neha Kapoor", 45, "Luxury", 4.9);
    d12.isAvailable = false;
    d12.completedRides = 156;
    drivers.push_back(d12);

    return drivers;
}

double CityGraphGenerator::toRad(double degrees) {
    return degrees * (M_PI / 180.0);
}

double CityGraphGenerator::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth's radius in km
    double dLat = toRad(lat2 - lat1);
    double dLon = toRad(lon2 - lon1);

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(toRad(lat1)) * std::cos(toRad(lat2)) *
               std::sin(dLon / 2) * std::sin(dLon / 2);

    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return R * c;
}

std::string CityGraphGenerator::getOrdinal(int n) {
    const std::vector<std::string> suffixes = {"th", "st", "nd", "rd"};
    int v = n % 100;
    if (v >= 11 && v <= 13) {
        return suffixes[0];
    }
    int lastDigit = n % 10;
    if (lastDigit >= 1 && lastDigit <= 3) {
        return suffixes[lastDigit];
    }
    return suffixes[0];
}

CityData* CityGraphGenerator::generateCityGraph(int numNodes) {
    CityData* cityData = new CityData();
    cityData->graph = new Graph(numNodes);

    std::vector<std::string> allNames = getAllLocationNames();
    std::vector<NodeData> nodeData;

    int sectorsPerSide = static_cast<int>(std::ceil(std::sqrt(numNodes / 3.0)));

    // Create nodes with realistic geographic clustering
    std::uniform_real_distribution<> dis(-0.5, 0.5);

    for (int i = 0; i < numNodes; i++) {
        int sector = i / 3;
        int sectorRow = sector / sectorsPerSide;
        int sectorCol = sector % sectorsPerSide;
        int subPosition = i % 3;

        double baseLat = 40.7128 + (sectorRow * 0.04);
        double baseLon = -74.0060 + (sectorCol * 0.04);

        double offsetLat = dis(gen) * 0.015 + (subPosition * 0.008);
        double offsetLon = dis(gen) * 0.015 + (subPosition * 0.008);

        double lat = baseLat + offsetLat;
        double lon = baseLon + offsetLon;

        std::string name = (i < static_cast<int>(allNames.size())) ? allNames[i] : "Location " + std::to_string(i);

        cityData->graph->addNode(i, name, lat, lon);
        nodeData.push_back({i, lat, lon, sector});
    }

    // Generate realistic road network
    createHighways(cityData->graph, nodeData, numNodes);
    createArterialRoads(cityData->graph, nodeData, numNodes);
    createLocalStreets(cityData->graph, nodeData, numNodes);
    createRingRoads(cityData->graph, nodeData, numNodes);
    createShortcuts(cityData->graph, nodeData, numNodes);

    // Ensure connectivity
    ensureConnectivity(cityData->graph);

    // Add drivers
    cityData->drivers = generateDrivers();

    return cityData;
}

void CityGraphGenerator::createHighways(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes) {
    std::vector<std::string> highwayNames = {"Interstate-95", "Highway-1", "Express Route", "Freeway", "Parkway"};
    std::uniform_int_distribution<> dis(0, highwayNames.size() - 1);

    // Main highway
    for (int i = 0; i < numNodes - 5; i += 5) {
        double distance = calculateDistance(
            nodeData[i].lat, nodeData[i].lon,
            nodeData[i + 5].lat, nodeData[i + 5].lon
        );
        double weight = distance * 80;
        std::string roadName = highwayNames[dis(gen)];
        graph->addEdge(i, i + 5, weight, roadName);
    }

    // Vertical highway
    int verticalStep = static_cast<int>(std::ceil(std::sqrt(numNodes)));
    for (int i = 0; i < numNodes - verticalStep; i += verticalStep) {
        if (i + verticalStep < numNodes) {
            double distance = calculateDistance(
                nodeData[i].lat, nodeData[i].lon,
                nodeData[i + verticalStep].lat, nodeData[i + verticalStep].lon
            );
            double weight = distance * 80;
            graph->addEdge(i, i + verticalStep, weight, "Highway North-South");
        }
    }
}

void CityGraphGenerator::createArterialRoads(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes) {
    std::vector<std::string> arterialNames = {"Main Street", "Broadway", "Avenue", "Boulevard", "Road"};
    std::uniform_int_distribution<> nameGen(0, arterialNames.size() - 1);
    std::uniform_real_distribution<> probGen(0.0, 1.0);

    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            double distance = calculateDistance(
                nodeData[i].lat, nodeData[i].lon,
                nodeData[j].lat, nodeData[j].lon
            );

            if (distance > 1.0 && distance < 4.0 && probGen(gen) < 0.3) {
                double weight = distance * 100;
                std::string roadName = arterialNames[nameGen(gen)];
                graph->addEdge(i, j, weight, roadName);
            }
        }
    }
}

void CityGraphGenerator::createLocalStreets(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes) {
    std::vector<std::string> streetNames = {"Street", "Lane", "Drive", "Court", "Way", "Place", "Circle"};
    std::uniform_int_distribution<> nameGen(0, streetNames.size() - 1);
    std::uniform_int_distribution<> numGen(1, 100);
    std::uniform_real_distribution<> probGen(0.0, 1.0);

    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            double distance = calculateDistance(
                nodeData[i].lat, nodeData[i].lon,
                nodeData[j].lat, nodeData[j].lon
            );

            if (distance < 1.5 && probGen(gen) < 0.5) {
                double weight = distance * 120;
                int streetNumber = numGen(gen);
                std::string roadName = std::to_string(streetNumber) + getOrdinal(streetNumber) + " " + streetNames[nameGen(gen)];
                graph->addEdge(i, j, weight, roadName);
            }
        }
    }
}

void CityGraphGenerator::createRingRoads(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes) {
    // Find center of city
    double centerLat = 0, centerLon = 0;
    for (const auto& node : nodeData) {
        centerLat += node.lat;
        centerLon += node.lon;
    }
    centerLat /= numNodes;
    centerLon /= numNodes;

    // Calculate distance from center for each node
    std::vector<std::pair<double, int>> nodesWithDistance;
    for (const auto& node : nodeData) {
        double dist = calculateDistance(node.lat, node.lon, centerLat, centerLon);
        nodesWithDistance.push_back({dist, node.id});
    }

    // Sort by distance from center
    std::sort(nodesWithDistance.begin(), nodesWithDistance.end());

    // Inner ring
    int innerRingSize = numNodes / 3;
    for (int i = 0; i < innerRingSize - 1; i++) {
        int nodeId1 = nodesWithDistance[i].second;
        int nodeId2 = nodesWithDistance[i + 1].second;

        double distance = calculateDistance(
            nodeData[nodeId1].lat, nodeData[nodeId1].lon,
            nodeData[nodeId2].lat, nodeData[nodeId2].lon
        );

        if (distance < 3.0) {
            double weight = distance * 90;
            graph->addEdge(nodeId1, nodeId2, weight, "Inner Ring Road");
        }
    }

    // Outer ring
    int outerRingStart = (numNodes * 2) / 3;
    for (int i = outerRingStart; i < numNodes - 1; i++) {
        int nodeId1 = nodesWithDistance[i].second;
        int nodeId2 = nodesWithDistance[i + 1].second;

        double distance = calculateDistance(
            nodeData[nodeId1].lat, nodeData[nodeId1].lon,
            nodeData[nodeId2].lat, nodeData[nodeId2].lon
        );

        if (distance < 4.0) {
            double weight = distance * 90;
            graph->addEdge(nodeId1, nodeId2, weight, "Outer Ring Road");
        }
    }
}

void CityGraphGenerator::createShortcuts(Graph* graph, const std::vector<NodeData>& nodeData, int numNodes) {
    std::vector<std::string> shortcutNames = {"Bridge", "Tunnel", "Overpass", "Underpass", "Connector"};
    std::uniform_int_distribution<> nodeDis(0, numNodes - 1);
    std::uniform_int_distribution<> nameDis(0, shortcutNames.size() - 1);

    int numShortcuts = std::min(10, numNodes / 5);

    for (int i = 0; i < numShortcuts; i++) {
        int node1 = nodeDis(gen);
        int node2 = nodeDis(gen);

        if (node1 != node2) {
            double distance = calculateDistance(
                nodeData[node1].lat, nodeData[node1].lon,
                nodeData[node2].lat, nodeData[node2].lon
            );

            if (distance > 2.0 && distance < 6.0) {
                double weight = distance * 85;
                std::string roadName = shortcutNames[nameDis(gen)] + " " + std::to_string(i + 1);
                graph->addEdge(node1, node2, weight, roadName);
            }
        }
    }
}

void CityGraphGenerator::ensureConnectivity(Graph* graph) {
    int n = graph->getNumVertices();
    std::vector<int> parent(n);
    for (int i = 0; i < n; i++) {
        parent[i] = i;
    }

    std::function<int(int)> find = [&](int x) -> int {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    };

    auto unionSets = [&](int x, int y) {
        int px = find(x);
        int py = find(y);
        if (px != py) {
            parent[px] = py;
        }
    };

    // Find connected components
    for (int i = 0; i < n; i++) {
        std::vector<Edge> neighbors = graph->getAdjacentNodes(i);
        for (const auto& edge : neighbors) {
            unionSets(i, edge.destination);
        }
    }

    // Connect disconnected components
    std::unordered_map<int, std::vector<int>> components;
    for (int i = 0; i < n; i++) {
        int root = find(i);
        components[root].push_back(i);
    }

    std::vector<std::vector<int>> componentList;
    for (const auto& pair : components) {
        componentList.push_back(pair.second);
    }

    // Connect each component to the next
    for (size_t i = 0; i < componentList.size() - 1; i++) {
        int node1 = componentList[i][0];
        int node2 = componentList[i + 1][0];

        Node n1 = graph->getNode(node1);
        Node n2 = graph->getNode(node2);

        double distance = calculateDistance(n1.latitude, n1.longitude, n2.latitude, n2.longitude);
        double weight = distance * 100;

        graph->addEdge(node1, node2, weight, "Connector Highway " + std::to_string(i + 1));
    }
}

} // namespace RideSharing
