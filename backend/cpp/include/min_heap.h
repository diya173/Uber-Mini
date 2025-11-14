/**
 * min_heap.h
 *
 * Min-Heap Priority Queue implementation for Dijkstra's algorithm
 * Used to efficiently get the node with minimum distance
 *
 * Time Complexity:
 *   - Insert: O(log n)
 *   - ExtractMin: O(log n)
 *   - DecreaseKey: O(log n)
 * Space Complexity: O(n)
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>
#include <unordered_map>
#include <limits>
#include <string>

namespace RideSharing {

// HeapNode represents an element in the priority queue
struct HeapNode {
    int vertex;           // Node/vertex ID
    double distance;      // Current shortest distance from source

    HeapNode(int v = -1, double d = std::numeric_limits<double>::infinity())
        : vertex(v), distance(d) {}

    bool operator>(const HeapNode& other) const {
        return distance > other.distance;
    }
};

class MinHeap {
private:
    std::vector<HeapNode> heap;
    std::unordered_map<int, int> positions; // Maps vertex to heap position
    std::vector<std::string> operationLogs; // Logs for visualization

    // Helper functions
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

    void swap(int i, int j);
    void heapifyUp(int i);
    void heapifyDown(int i);

    void logOperation(const std::string& operation);

public:
    MinHeap();

    // Insert a new node into the heap
    void insert(int vertex, double distance);

    // Extract the node with minimum distance
    HeapNode extractMin();

    // Decrease the distance value of a vertex
    void decreaseKey(int vertex, double newDistance);

    // Check if heap is empty
    bool isEmpty() const { return heap.empty(); }

    // Get heap size
    int size() const { return heap.size(); }

    // Check if vertex is in heap
    bool contains(int vertex) const;

    // Get operation logs for visualization
    std::vector<std::string> getLogs() const { return operationLogs; }

    // Clear logs
    void clearLogs() { operationLogs.clear(); }

    // Get current heap state as string (for debugging)
    std::string toString() const;
};

} // namespace RideSharing

#endif // MIN_HEAP_H
