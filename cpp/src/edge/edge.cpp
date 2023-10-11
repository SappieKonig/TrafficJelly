#include "edge/edge.h"
#include "node/node.h"

#include <iostream>

Edge::Edge(Node& inNode, Node& outNode, std::string label, float length, float speedLimit)
    : inNode(inNode), outNode(outNode), label(std::move(label)), length(length), speedLimit(speedLimit)
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

std::vector<std::unique_ptr<Car>> Edge::getExitingCars() {
    std::vector<std::unique_ptr<Car>> exiting_cars;
    // Loop from back to front
    for (auto car = cars.rbegin(); car != cars.rend(); ++car) {
        if ((*car)->getX() > length) {
            exiting_cars.push_back(std::move(*car));
            cars.pop_back();
        }
    }
    return exiting_cars;
}
