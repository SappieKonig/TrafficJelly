#include "edge.h"
#include "node.h"
#include "car.h"

#include <algorithm>
#include <iostream>

Edge::Edge(Node& inNode, Node& outNode, std::string label)
    : inNode(inNode), outNode(outNode), label(label)
{
    inNode.outEdges.push_back(*this);
    outNode.inEdges.push_back(*this);
}

std::string Edge::getLabel() const
{
    return label;
}

BasicRoad::BasicRoad(Node& inNode, Node& outNode, std::string label, float length)
    : Edge(inNode, outNode, label), length(length)
{

}

void BasicRoad::step(float dt)
{
    Observation observation;
    for (auto& car : cars)
    {
        car->step(observation, dt);
    }

    std::sort(cars.begin(), cars.end());

    // Transfers cars to nodes. Does not immediately step at node.
    if (!cars.empty())
    {
        while (cars[-1]->getX() < length) // I dislike having to give the position up.
        {
            outNode.addCar(std::move(cars[-1]));
            cars.pop_back();
        }
    }
}

std::string BasicRoad::toString() const
{
    return "BasicRoad:" + label + "," + inNode.getLabel() + "," + outNode.getLabel() + "," + std::to_string(length);
}
