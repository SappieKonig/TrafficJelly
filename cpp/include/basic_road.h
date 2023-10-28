//
// Created by sappie on 10-10-23.
//

#ifndef CPP_SIMPLE_ROAD_H
#define CPP_SIMPLE_ROAD_H

#include "edge.h"
#include "node.h"


class BasicRoad : public Edge
{
public:
    BasicRoad(Node& inNode, Node& outNode, std::string label, float length, float speedLimit, int nLanes);
    void enterCar(std::unique_ptr<Car>&& car) override;
    void setActions() override;
    std::string toString() const override;
    int nLanes;
};


#endif //CPP_SIMPLE_ROAD_H
