//
// Created by sappie on 11-10-23.
//
#include <random>

#include "node/node.h"

#ifndef TRAFFICJELLY_BASIC_CITY_H
#define TRAFFICJELLY_BASIC_CITY_H


class BasicCity : public Node {
private:
    int population;
    std::mt19937 rng;
public:
    BasicCity(std::string label, int population);
    int getPopulation() { return population; }
    void collectCars() override;
    void distributeCars() override;
    void step(float dt) override {}
};


#endif //TRAFFICJELLY_BASIC_CITY_H
