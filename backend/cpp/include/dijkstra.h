/**
 * dijkstra.h
 *
 * Dijkstra's Algorithm implementation for finding shortest paths
 * Uses MinHeap for efficient minimum distance extraction
 *
 * Time Complexity: O((V + E) log V) with binary heap
 * Space Complexity: O(V)
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "min_heap.h"
#include <vector>
#include <string>

namespace RideSharing {

// Result structure for Dijkstra's algorithm
struct DijkstraResult {
    std::vector<double> distances;      // Shortest distances from source
    std::vector<int> predecessors;      // Previous node in shortest path
    std::vector<std::string> logs;      // Algorithm execution logs
    bool success;                        // Whether algorithm completed successfully
    std::string errorMessage;           // Error message if failed

    DijkstraResult() : success(true) {}
};

// Path structure for reconstructed route
struct PathResult {
    std::vector<int> path;              // Sequence of nodes from source to destination
    double totalDistance;               // Total distance of the path
    double estimatedTime;               // Estimated time (assuming average speed)
    std::vector<std::string> roadNames; // Names of roads in the path
    bool found;                         // Whether path was found

    PathResult() : totalDistance(0.0), estimatedTime(0.0), found(false) {}
};

class Dijkstra {
private:
    const Graph& graph;
    std::vector<std::string> executionLogs;

    void logStep(const std::string& message);

public:
    explicit Dijkstra(const Graph& g);

    // Run Dijkstra's algorithm from a source node
    DijkstraResult findShortestPaths(int source);

    // Find shortest path between two specific nodes
    PathResult findShortestPath(int source, int destination);

    // Get execution logs for visualization
    std::vector<std::string> getLogs() const { return executionLogs; }

    // Clear execution logs
    void clearLogs() { executionLogs.clear(); }

    // Reconstruct path from source to destination using predecessors
    static std::vector<int> reconstructPath(int source, int destination,
                                           const std::vector<int>& predecessors);

    // Calculate estimated time based on distance and average speed
    static double calculateETA(double distance, double avgSpeedKmh = 40.0);
};

} // namespace RideSharing

#endif // DIJKSTRA_H
