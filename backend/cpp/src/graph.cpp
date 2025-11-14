/**
 * graph.cpp
 *
 * Implementation of Graph data structure using adjacency list
 */

#include "include/graph.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace RideSharing {

Graph::Graph(int vertices) : numVertices(vertices) {
    adjacencyList.resize(vertices);
}

void Graph::addEdge(int src, int dest, double weight, const std::string& roadName) {
    // Validate input
    if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
        throw std::out_of_range("Invalid vertex index");
    }
    if (weight < 0) {
        throw std::invalid_argument("Edge weight cannot be negative");
    }

    // Add bidirectional edge
    adjacencyList[src].emplace_back(dest, weight, roadName);
    adjacencyList[dest].emplace_back(src, weight, roadName);
}

void Graph::addDirectedEdge(int src, int dest, double weight, const std::string& roadName) {
    // Validate input
    if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
        throw std::out_of_range("Invalid vertex index");
    }
    if (weight < 0) {
        throw std::invalid_argument("Edge weight cannot be negative");
    }

    // Add unidirectional edge
    adjacencyList[src].emplace_back(dest, weight, roadName);
}

void Graph::addNode(int id, const std::string& name, double lat, double lon) {
    if (id < 0 || id >= numVertices) {
        throw std::out_of_range("Invalid node ID");
    }
    nodes[id] = Node(id, name, lat, lon);
}

const std::vector<Edge>& Graph::getAdjacentNodes(int vertex) const {
    if (vertex < 0 || vertex >= numVertices) {
        throw std::out_of_range("Invalid vertex index");
    }
    return adjacencyList[vertex];
}

const Node& Graph::getNode(int id) const {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        throw std::out_of_range("Node not found");
    }
    return it->second;
}

bool Graph::nodeExists(int id) const {
    return nodes.find(id) != nodes.end();
}

bool Graph::validate() const {
    // Check if all referenced nodes exist
    for (int i = 0; i < numVertices; ++i) {
        for (const auto& edge : adjacencyList[i]) {
            if (edge.destination < 0 || edge.destination >= numVertices) {
                return false;
            }
            if (edge.weight < 0) {
                return false;
            }
        }
    }
    return true;
}

std::string Graph::toJSON() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "{\"numVertices\":" << numVertices << ",\"nodes\":[";

    bool firstNode = true;
    for (const auto& pair : nodes) {
        if (!firstNode) oss << ",";
        firstNode = false;

        const Node& node = pair.second;
        oss << "{\"id\":" << node.id
            << ",\"name\":\"" << node.name << "\""
            << ",\"latitude\":" << node.latitude
            << ",\"longitude\":" << node.longitude << "}";
    }

    oss << "],\"edges\":[";
    bool firstEdge = true;
    for (int i = 0; i < numVertices; ++i) {
        for (const auto& edge : adjacencyList[i]) {
            // Only add each edge once (avoid duplicates for bidirectional edges)
            if (i < edge.destination) {
                if (!firstEdge) oss << ",";
                firstEdge = false;

                oss << "{\"source\":" << i
                    << ",\"destination\":" << edge.destination
                    << ",\"weight\":" << edge.weight
                    << ",\"roadName\":\"" << edge.roadName << "\"}";
            }
        }
    }

    oss << "]}";
    return oss.str();
}

} // namespace RideSharing
