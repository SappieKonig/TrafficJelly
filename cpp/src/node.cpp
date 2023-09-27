#include "node.h"
#include "edge.h"

#include <iostream>

Node::Node(std::string label)
    : label(label)
{

}

void Node::addCar(std::unique_ptr<Car>&&  car)
{
    cars.push_back(std::move(car));
}

std::string Node::getLabel() const
{
    return label;
}

BasicCity::BasicCity(std::string label)
    : Node(label)
{

}

void BasicCity::step(float dt)
{
    Observation observation;
    for (auto& car : cars)
    {
        car->step(observation, dt);
    }
}

std::string BasicCity::toString() const
{
    return "BasicCity:" + label;
}
