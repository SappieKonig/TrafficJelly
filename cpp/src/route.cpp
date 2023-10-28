#include <vector>
#include <queue>
#include "route.h"
#include "node/node.h"
#include "edge/edge.h"

// This is a custom comparator for the priority queue
struct ComparePair {
    bool operator()(const std::pair<float, int>& p1, const std::pair<float, int>& p2) {
        return p1.first > p2.first;
    }
};

std::vector<std::vector<int>> computeMapping(std::vector<std::shared_ptr<Node>>& nodes) {
    int n = nodes.size();
    std::vector<std::vector<int>> arr(n, std::vector<int>(n, -1));

    for (auto startNode : nodes) {
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, ComparePair> pq;
        std::vector<float> dist(n, std::numeric_limits<float>::infinity());
        std::vector<int> prev(n, -1);

        pq.emplace(0.0f, startNode->id);
        dist[startNode->id] = 0.0f;

        while (!pq.empty()) {
            auto [currDist, currNodeID] = pq.top();
            pq.pop();

            auto currNode = nodes[currNodeID];

            for (Edge& edgeRef : currNode->outEdges) {
                Edge& edge = edgeRef;
                float alt = currDist + edge.getExpectedCrossingTime();
                int nextNodeID = edge.outNode.id;

                if (alt < dist[nextNodeID]) {
                    dist[nextNodeID] = alt;
                    prev[nextNodeID] = currNodeID;
                    pq.push({alt, nextNodeID});

                    // Update the arr with this node as the "stepping stone" to nextNode
                    int temp = nextNodeID;
                    while (prev[temp] != startNode->id) {
                        temp = prev[temp];
                    }
                    arr[nextNodeID][startNode->id] = temp;
                }
            }
        }
    }

    return arr;
}


std::vector<int> reconstructPath(const std::vector<std::vector<int>>& arr, int startNodeId, int endNodeId) {
    std::vector<int> path;
    if (arr[endNodeId][startNodeId] == -1) {
        // No path exists from startNodeId to endNodeId.
        return path;
    }

    path.push_back(startNodeId);
    while (startNodeId != endNodeId) {
        startNodeId = arr[endNodeId][startNodeId];
        path.push_back(startNodeId);
    }

    return path;
}

