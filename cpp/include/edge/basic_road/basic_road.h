//
// Created by sappie on 10-10-23.
//

#ifndef CPP_SIMPLE_ROAD_H
#define CPP_SIMPLE_ROAD_H

#include "edge/edge.h"
#include "node.h"


class BasicRoad : public Edge
{
private:
    float const length;
public:
    BasicRoad(Node& inNode, Node& outNode, std::string label, float length, int nLanes);
    void initialize() override;
    std::string toString() const override;
    void enterCar(std::unique_ptr<Car>&& car) override;
    void setActions() override;
    int nLanes;
};


#endif //CPP_SIMPLE_ROAD_H
