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
    BasicCity(std::string label, int population, float x, float y, std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device);
    int getPopulation() { return population; }
    void collectCars() override;
    void distributeCars() override;
    void spawnCar();
    void step(float dt) override {
        // Assume 20% of population commutes
        float p_spawn = (float) population * 0.2f / 86400.0f * dt;
        std::uniform_real_distribution<> dis(0.0f, 1.0f);
        if (dis(rng) < p_spawn) {
            spawnCar();
        }
    }
    std::string toString() const override;
};


#endif //TRAFFICJELLY_BASIC_CITY_H
