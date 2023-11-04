#ifndef NODE_H
#define NODE_H

#include <list>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <random>

#include "utils.h"
#include "car.h"
#include "edge/edge.h"
#include <tuple>
/*
 * This is a node for the internal graph of TrafficModel.
 * Each Node instance represents a city or crossroad.
 * It carries a Car instance from any inEdge to an outEdge of its choice (after indefinite hold).
 * It may also act as source or sink of cars on the graph.
 */

class Node {
    std::string const label;
public:
    float x, y;
    std::vector<std::tuple<int, int, float, float>> travelStats;
    int population;
    int id;
    Node(std::string label, float x, float y, int population);
    std::string getLabel() { return label; }
    std::vector<std::reference_wrapper<Edge>> inEdges; // ref
    std::vector<std::reference_wrapper<Edge>> outEdges;
    std::vector<std::unique_ptr<Car>> storedCars;
    void spawnCar(std::vector<int> path, float global_time) {
        std::unique_ptr<Car> car = std::make_unique<Car>(path, global_time);
        storedCars.push_back(std::move(car));
    }
    void collectCars();
    virtual void distributeCars() = 0;
    virtual void step(float dt) = 0;
    int getID() const { return id; }
    void setID(int id) { this->id = id; }
    float distanceTo(Node& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
    std::tuple<float, float> getPosition() const { return std::make_tuple(x, -y); }
    int getNCars() const { return storedCars.size(); }
};

#endif
