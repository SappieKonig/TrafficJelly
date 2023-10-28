#include "edge/basic_road/basic_road.h"

#include <string>
#include <utility>
#include "edge/basic_road/basic_road_observation.h"
#include "car.h"
#include "algorithm"
#include "node/node.h"

BasicRoad::BasicRoad(Node& inNode, Node& outNode, std::string label, float length, float speedLimit, int nLanes)
        : Edge(inNode, outNode, std::move(label), length, speedLimit),  nLanes(nLanes)
{

}

void BasicRoad::setActions()
{
    float margin = 200;  // update to be relative to speed (or don't)
    for (auto base_car = cars.begin(); base_car != cars.end(); ++base_car) {
        // Collect cars in front and in back
        std::vector<std::reference_wrapper<Car>> nearby_cars;
        // Forward
        auto forward_car = base_car;
        for (auto car = forward_car++; car != cars.end(); ++car) {
            if ((*car)->getX() - (*base_car)->getX() > margin) {
                break;
            }
            nearby_cars.push_back(std::ref(**car));
        }
        // Backward
        auto backward_car = base_car;
        for (auto car = backward_car--; car != cars.begin(); --car) {
            if ((*car)->getX() - (*base_car)->getX() < -margin) {
                break;
            }
            nearby_cars.push_back(std::ref(**car));
        }
        Observation obs = {nearby_cars, *base_car, nLanes};
        (*base_car)->updateAction(obs);
    }

}

std::string BasicRoad::toString() const
{
    return "BasicRoad:" + label + "," + inNode.getLabel() + "," + outNode.getLabel() + "," + std::to_string(length) + "," + std::to_string(speedLimit) + "," + std::to_string(nLanes);
}

void BasicRoad::enterCar(std::unique_ptr<Car>&& car)
{
    car->syncCarToEdge(speedLimit);
    cars.push_front(std::move(car));
}
