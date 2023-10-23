//
// Created by sappie on 11-10-23.
//

#include "node/basic_city.h"
#include "car.h"

#include <utility>

BasicCity::BasicCity(std::string label, int population, float x, float y)
    : Node(std::move(label), x, y), population(population)
{
    std::random_device rd;
    rng = std::mt19937(rd());
}

void BasicCity::distributeCars() {
    for (auto& car : storedCars) {
        // Choose a random edge to put the car on
        std::uniform_int_distribution<> dis(0, (int) outEdges.size() - 1);
        int index = dis(rng);
        outEdges[index].get().enterCar(std::move(car));
    }
    storedCars.clear();
}

void BasicCity::spawnCar() {
    std::unique_ptr<Car> car = std::make_unique<Car>();
    // Put the car on the storedCars list
    storedCars.push_back(std::move(car));
}
