#include "node/basic_city.h"
#include <string>

BasicCity::BasicCity(std::string label, int population)
    : Node(label), population(population)
{
    std::random_device rd;
    rng = std::mt19937(rd());
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
        // Choose a random edge to put the car on
        std::uniform_int_distribution<> dis(0, outEdges.size() - 1);
        int index = dis(rng);
        outEdges[index].get().enterCar(std::move(car));
    }
    storedCars.clear();
}

std::string BasicCity::toString() const
{
    return "BasicCity:" + label + "," + std::to_string(population);
}

