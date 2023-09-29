#include "node.h"
#include "edge.h"

#include <iostream>

Node::Node(std::string label)
    : label(label)
{

}

Node::~Node()
{

}

void Node::enterCar(std::unique_ptr<Car>&&  car)
{
    cars.push_back(std::move(car));
}

std::string Node::getLabel() const
{
    return label;
}

BasicCity::BasicCity(std::string label, std::vector<Route>& routes, std::random_device& device)
    : Node(label), carFactory(routes, device)
{

}

void BasicCity::initialize()
{
    //cars.push_back(std::move(carFactory.createCar()));
}

void BasicCity::step(float dt, std::shared_ptr<Visualizer> visualizer)
{
    Observation observation(true);
    // TODO: Construct observation with useful data for the cars.
    for (auto& car : cars)
    {
        car->step(observation, dt);

        VisualizerData visualizerData;
        visualizerData.objectLabel = label;
        visualizerData.position = car->getX();
        visualizer->update(visualizerData);

        // Transferring cars is eagerly implemented. This part is sticky. Fix the const shit.
        if (!car->isWaiting())
        {
            Checkpoint& checkpoint = *car->getTargetCheckpoint();
            Edge& edge = checkpoint.edgeToNode;
            edge.enterCar(std::move(car));
        }
    }
}

void BasicCity::spawnCar()
{
    cars.push_back(std::move(carFactory.createCar()));
}

std::string BasicCity::toString() const
{
    return "BasicCity:" + label;
}
