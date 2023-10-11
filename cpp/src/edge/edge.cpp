#include "edge/edge.h"
#include "node.h"

#include <iostream>

Edge::Edge(Node& inNode, Node& outNode, std::string label)
    : inNode(inNode), outNode(outNode), label(label)
{
    inNode.outEdges.emplace_back(*this);
    outNode.inEdges.emplace_back(*this);
}

Edge::~Edge()
{

}

std::string Edge::getLabel() const
{
    return label;
}

void Edge::sortCars() {
    // We sort cars by iterating over them, and if the order is wrong, we swap them.
    // After a swap, we keep going with the car with the least distance,
    // because it may have been overtaken twice (or more).
    // Since we only have local changes, we can do this in O(n),
    // with practically zero memory cost.
    for (auto it = cars.begin(); it != cars.end();) {
        auto next = it;
        next++;
        if (next != cars.end() && (*it) > (*next)) {
            std::iter_swap(it, next);
        }
        it = next;
    }
}


void Edge::updateCars(float dt)
{
    for (auto& car : cars)
    {
        car->step(dt);
    }
}

