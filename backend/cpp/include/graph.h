/**
 * graph.h
 *
 * Graph implementation using adjacency list representation
 * Stores city map with nodes (locations) and weighted edges (roads)
 *
 * Time Complexity: O(V + E) for storage
 * Space Complexity: O(V + E)
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace RideSharing {

// Edge structure representing a road between two locations
struct Edge {
    int destination;      // Destination node ID
    double weight;        // Distance/time between nodes
    std::string roadName; // Optional road name for display

    Edge(int dest, double w, const std::string& name = "")
        : destination(dest), weight(w), roadName(name) {}
};

// Node structure representing a location in the city
struct Node {
    int id;
    std::string name;
    double latitude;
    double longitude;

    Node(int nodeId = 0, const std::string& nodeName = "",
         double lat = 0.0, double lon = 0.0)
        : id(nodeId), name(nodeName), latitude(lat), longitude(lon) {}
};

class Graph {
private:
    int numVertices;
    std::vector<std::vector<Edge>> adjacencyList;
    std::unordered_map<int, Node> nodes;

public:
    // Constructor
    explicit Graph(int vertices);

    // Add a bidirectional edge (road between two locations)
    void addEdge(int src, int dest, double weight, const std::string& roadName = "");

    // Add a unidirectional edge (one-way road)
    void addDirectedEdge(int src, int dest, double weight, const std::string& roadName = "");

    // Add node information
    void addNode(int id, const std::string& name, double lat, double lon);

    // Get adjacent nodes
    const std::vector<Edge>& getAdjacentNodes(int vertex) const;

    // Get node information
    const Node& getNode(int id) const;

    // Check if node exists
    bool nodeExists(int id) const;

    // Get total number of vertices
    int getNumVertices() const { return numVertices; }

    // Get all nodes
    const std::unordered_map<int, Node>& getAllNodes() const { return nodes; }

    // Validate graph integrity
    bool validate() const;

    // Export graph to JSON string
    std::string toJSON() const;
};

} // namespace RideSharing

#endif // GRAPH_H
