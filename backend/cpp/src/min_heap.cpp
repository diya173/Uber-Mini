/**
 * min_heap.cpp
 *
 * Implementation of Min-Heap Priority Queue
 */

#include "include/min_heap.h"
#include <sstream>
#include <iomanip>

namespace RideSharing {

MinHeap::MinHeap() {
    heap.reserve(100);
}

void MinHeap::swap(int i, int j) {
    // Update positions map
    positions[heap[i].vertex] = j;
    positions[heap[j].vertex] = i;

    // Swap heap nodes
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void MinHeap::heapifyUp(int i) {
    while (i > 0 && heap[parent(i)].distance > heap[i].distance) {
        std::ostringstream log;
        log << "HeapifyUp: Swapping node " << heap[i].vertex
            << " (dist=" << std::fixed << std::setprecision(2) << heap[i].distance
            << ") with parent " << heap[parent(i)].vertex
            << " (dist=" << heap[parent(i)].distance << ")";
        logOperation(log.str());

        swap(i, parent(i));
        i = parent(i);
    }
}

void MinHeap::heapifyDown(int i) {
    int minIndex = i;
    int left = leftChild(i);
    int right = rightChild(i);

    if (left < heap.size() && heap[left].distance < heap[minIndex].distance) {
        minIndex = left;
    }
    if (right < heap.size() && heap[right].distance < heap[minIndex].distance) {
        minIndex = right;
    }

    if (minIndex != i) {
        std::ostringstream log;
        log << "HeapifyDown: Swapping node " << heap[i].vertex
            << " (dist=" << std::fixed << std::setprecision(2) << heap[i].distance
            << ") with child " << heap[minIndex].vertex
            << " (dist=" << heap[minIndex].distance << ")";
        logOperation(log.str());

        swap(i, minIndex);
        heapifyDown(minIndex);
    }
}

void MinHeap::insert(int vertex, double distance) {
    std::ostringstream log;
    log << "Insert: Adding vertex " << vertex
        << " with distance " << std::fixed << std::setprecision(2) << distance;
    logOperation(log.str());

    HeapNode node(vertex, distance);
    heap.push_back(node);
    int index = heap.size() - 1;
    positions[vertex] = index;

    heapifyUp(index);
}

HeapNode MinHeap::extractMin() {
    if (heap.empty()) {
        return HeapNode(-1, std::numeric_limits<double>::infinity());
    }

    HeapNode minNode = heap[0];

    std::ostringstream log;
    log << "ExtractMin: Removing vertex " << minNode.vertex
        << " with distance " << std::fixed << std::setprecision(2) << minNode.distance;
    logOperation(log.str());

    // Move last element to root
    heap[0] = heap.back();
    positions[heap[0].vertex] = 0;
    heap.pop_back();

    // Remove min node from positions
    positions.erase(minNode.vertex);

    if (!heap.empty()) {
        heapifyDown(0);
    }

    return minNode;
}

void MinHeap::decreaseKey(int vertex, double newDistance) {
    auto it = positions.find(vertex);
    if (it == positions.end()) {
        // Vertex not in heap, insert it
        insert(vertex, newDistance);
        return;
    }

    int index = it->second;
    double oldDistance = heap[index].distance;

    std::ostringstream log;
    log << "DecreaseKey: Updating vertex " << vertex
        << " from distance " << std::fixed << std::setprecision(2) << oldDistance
        << " to " << newDistance;
    logOperation(log.str());

    heap[index].distance = newDistance;
    heapifyUp(index);
}

bool MinHeap::contains(int vertex) const {
    return positions.find(vertex) != positions.end();
}

void MinHeap::logOperation(const std::string& operation) {
    operationLogs.push_back(operation);
}

std::string MinHeap::toString() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < heap.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "(" << heap[i].vertex << ":" << std::fixed << std::setprecision(2)
            << heap[i].distance << ")";
    }
    oss << "]";
    return oss.str();
}

} // namespace RideSharing
