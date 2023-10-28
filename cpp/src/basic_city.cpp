#include "basic_city.h"
#include <string>
#include <utility>

BasicCity::BasicCity(std::string label, int population, float x, float y, std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device)
    : Node(std::move(label), x, y), population(population), carFactory(routes, device)
{

}

void BasicCity::spawnCar()
{
    storedCars.push_back(carFactory.createCar());
}

void BasicCity::collectCars()
{
    for (auto& edge : inEdges)
    {
        auto exitingCars = edge.get().getExitingCars();
        storedCars.insert(storedCars.end(), std::make_move_iterator(exitingCars.begin()), std::make_move_iterator(exitingCars.end()));
    }
}

void BasicCity::distributeCars()
{
    for (auto& car : storedCars)
    {
        // Choose a random edge to put the car on (this should work via the route planner --> TODO: implement RandomRoutePlanner)
        //std::uniform_int_distribution<> dis(0, outEdges.size() - 1);
        //int index = dis(rng);
        //outEdges[index].get().enterCar(std::move(car));
        if (!car->hasTerminated())
        {
            car->nextCheckpoint();
            std::unique_ptr<Checkpoint>& checkpoint = car->getTargetCheckpoint(); // Must be seperate for implementing waiting time
            checkpoint->edgeToNode->enterCar(std::move(car)); // TODO: Implement waiting time, and yes, we will need a list rather than a vector.
        }
    }
    storedCars.clear();
}

std::string BasicCity::toString() const
{
    return "BasicCity:" + label + "," + std::to_string(population);
}
