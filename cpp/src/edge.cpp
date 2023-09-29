#include "edge.h"
#include "node.h"
#include "car.h"

#include <algorithm>
#include <iostream>
#include <memory>

Edge::Edge(Node& inNode, Node& outNode, std::string label)
    : inNode(inNode), outNode(outNode), label(label)
{
    inNode.outEdges.push_back(*this);
    outNode.inEdges.push_back(*this);
}

Edge::~Edge()
{

}

std::string Edge::getLabel() const
{
    return label;
}

BasicRoad::BasicRoad(Node& inNode, Node& outNode, std::string label, float length)
    : Edge(inNode, outNode, label), length(length)
{

}

void BasicRoad::initialize()
{
    // Maybe do some initial distribution
}

void BasicRoad::step(float dt, std::shared_ptr<Visualizer> visualizer)
{
    Observation observation(false);
    // TODO: Construct observation with useful data for the cars.
    for (auto& car : cars)
    {
        car->step(observation, dt);

        VisualizerData visualizerData;
        visualizerData.objectLabel = label;
        visualizerData.position = car->getX();
        visualizer->update(visualizerData);
    }

    std::sort(cars.begin(), cars.end());

    // Transfers cars to nodes. Does not immediately step at node.
    if (!cars.empty())
    {
    std::cout << "hi\n";
        while (cars.back()->getX() < length) // I dislike having to give the position up.
        {
            if (cars.back()->nextCheckpoint() != nullptr) // End journey if no checkpoint.
            {
    std::cout << "hip\n";
                outNode.enterCar(std::move(cars.back()));
            }
    std::cout << "hi\n";
            cars.pop_back();
        }
    std::cout << "hit\n";
    }
}

std::string BasicRoad::toString() const
{
    return "BasicRoad:" + label + "," + inNode.getLabel() + "," + outNode.getLabel() + "," + std::to_string(length);
}

void BasicRoad::enterCar(std::unique_ptr<Car>&& car)
{
    cars.push_back(std::move(car));
}

