//
// Created by sappie on 11-10-23.
//

#include "node/basic_city.h"
#include "car.h"

#include <utility>

BasicCity::BasicCity(std::string label, int population, float x, float y)
    : Node(std::move(label), x, y, population), population(population)
{
    std::random_device rd;
    rng = std::mt19937(rd());
}

void BasicCity::distributeCars() {
    for (auto& car : storedCars) {
        if (car->path[0] == id) {
            // remove the first node from the path
            car->path.erase(car->path.begin());
        }
        if (car->path.empty()) {
            travelStats.emplace_back(car->fromNodeID, car->toNodeID, car->age, car->global_time);
            continue;
        }
        int nextNode = car->path[0];
        for (auto& edge : outEdges) {
            if (edge.get().getOutNode().getID() == nextNode) {
                edge.get().enterCar(std::move(car));
                break;
            }
        }
    }
    storedCars.clear();
}