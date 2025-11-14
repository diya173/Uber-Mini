/**
 * dijkstra.cpp
 *
 * Implementation of Dijkstra's shortest path algorithm
 */

#include "include/dijkstra.h"
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace RideSharing {

Dijkstra::Dijkstra(const Graph& g) : graph(g) {}

void Dijkstra::logStep(const std::string& message) {
    executionLogs.push_back(message);
}

DijkstraResult Dijkstra::findShortestPaths(int source) {
    DijkstraResult result;
    executionLogs.clear();

    // Validate source
    if (!graph.nodeExists(source)) {
        result.success = false;
        result.errorMessage = "Source node does not exist";
        return result;
    }

    int n = graph.getNumVertices();
    result.distances.assign(n, std::numeric_limits<double>::infinity());
    result.predecessors.assign(n, -1);

    // Initialize
    result.distances[source] = 0.0;
    MinHeap pq;
    pq.insert(source, 0.0);

    std::ostringstream log;
    log << "Starting Dijkstra from node " << source;
    logStep(log.str());

    int nodesProcessed = 0;

    while (!pq.isEmpty()) {
        HeapNode current = pq.extractMin();
        int u = current.vertex;
        double dist = current.distance;

        // Skip if we've already found a better path
        if (dist > result.distances[u]) {
            continue;
        }

        nodesProcessed++;
        log.str("");
        log << "Processing node " << u << " with distance "
            << std::fixed << std::setprecision(2) << dist;
        logStep(log.str());

        // Explore neighbors
        try {
            const std::vector<Edge>& neighbors = graph.getAdjacentNodes(u);

            for (const Edge& edge : neighbors) {
                int v = edge.destination;
                double weight = edge.weight;
                double newDist = result.distances[u] + weight;

                if (newDist < result.distances[v]) {
                    log.str("");
                    log << "  Relaxing edge " << u << " -> " << v
                        << ": distance updated from "
                        << std::fixed << std::setprecision(2) << result.distances[v]
                        << " to " << newDist;
                    logStep(log.str());

                    result.distances[v] = newDist;
                    result.predecessors[v] = u;
                    pq.decreaseKey(v, newDist);
                }
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = std::string("Error processing node: ") + e.what();
            return result;
        }
    }

    log.str("");
    log << "Dijkstra completed. Processed " << nodesProcessed << " nodes.";
    logStep(log.str());

    // Copy heap logs
    result.logs = executionLogs;
    auto heapLogs = pq.getLogs();
    result.logs.insert(result.logs.end(), heapLogs.begin(), heapLogs.end());

    return result;
}

PathResult Dijkstra::findShortestPath(int source, int destination) {
    PathResult pathResult;
    executionLogs.clear();

    // Validate input
    if (!graph.nodeExists(source)) {
        pathResult.found = false;
        return pathResult;
    }
    if (!graph.nodeExists(destination)) {
        pathResult.found = false;
        return pathResult;
    }

    // Run Dijkstra
    DijkstraResult dijkstraResult = findShortestPaths(source);

    if (!dijkstraResult.success) {
        pathResult.found = false;
        return pathResult;
    }

    // Check if destination is reachable
    if (dijkstraResult.distances[destination] == std::numeric_limits<double>::infinity()) {
        pathResult.found = false;
        std::ostringstream log;
        log << "No path found from " << source << " to " << destination;
        logStep(log.str());
        return pathResult;
    }

    // Reconstruct path
    pathResult.path = reconstructPath(source, destination, dijkstraResult.predecessors);
    pathResult.totalDistance = dijkstraResult.distances[destination];
    pathResult.estimatedTime = calculateETA(pathResult.totalDistance);
    pathResult.found = true;

    // Get road names
    for (size_t i = 0; i < pathResult.path.size() - 1; ++i) {
        int from = pathResult.path[i];
        int to = pathResult.path[i + 1];

        const std::vector<Edge>& edges = graph.getAdjacentNodes(from);
        for (const Edge& edge : edges) {
            if (edge.destination == to) {
                pathResult.roadNames.push_back(edge.roadName);
                break;
            }
        }
    }

    std::ostringstream log;
    log << "Path found: ";
    for (size_t i = 0; i < pathResult.path.size(); ++i) {
        if (i > 0) log << " -> ";
        log << pathResult.path[i];
    }
    log << " (Distance: " << std::fixed << std::setprecision(2)
        << pathResult.totalDistance << " km, ETA: "
        << std::setprecision(1) << pathResult.estimatedTime << " min)";
    logStep(log.str());

    return pathResult;
}

std::vector<int> Dijkstra::reconstructPath(int source, int destination,
                                           const std::vector<int>& predecessors) {
    std::vector<int> path;

    // Backtrack from destination to source
    int current = destination;
    while (current != -1) {
        path.push_back(current);
        if (current == source) break;
        current = predecessors[current];
    }

    // Reverse to get path from source to destination
    std::reverse(path.begin(), path.end());

    return path;
}

double Dijkstra::calculateETA(double distance, double avgSpeedKmh) {
    // Convert to minutes
    return (distance / avgSpeedKmh) * 60.0;
}

} // namespace RideSharing
