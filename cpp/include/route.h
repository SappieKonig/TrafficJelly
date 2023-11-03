#ifndef ROUTE_H
#define ROUTE_H

#include "node/node.h"
#include <vector>

std::vector<std::vector<int>> computeMapping(std::vector<std::shared_ptr<Node>>& nodes);
std::vector<std::vector<float>> computeProbabilities(std::vector<int> populations);
std::vector<int> reconstructPath(const std::vector<std::vector<int>>& arr, int startNodeId, int endNodeId);

#endif
