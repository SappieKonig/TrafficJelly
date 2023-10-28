#include <random>
#include "node.h"

#ifndef TRAFFICJELLY_BASIC_CITY_H
#define TRAFFICJELLY_BASIC_CITY_H

class BasicCity : public Node
{
private:
    int population;
    std::mt19937 rng;
    BasicCarFactory carFactory;

public:
    BasicCity(std::string label, int population, std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device);
    int getPopulation() { return population; }
    void spawnCar();
    void collectCars() override;
    void distributeCars() override;
    void step(float dt) override {}
    std::string toString() const override;
};


#endif //TRAFFICJELLY_BASIC_CITY_H
