#include "node.h"
#include <iostream>
#include <utility>
#include <random>

Node::Node(std::string label, float x, float y)
    : label(std::move(label)), x(x), y(y)
{
    id = -1;
}


void Node::collectCars() {
    for (auto& edge : inEdges)
    {
        auto exitingCars = edge.get().getExitingCars();
        storedCars.insert(storedCars.end(), std::make_move_iterator(exitingCars.begin()), std::make_move_iterator(exitingCars.end()));
    }
}
