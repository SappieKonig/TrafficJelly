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
#include "edge.h"
#include "visualizer.h"
#include <tuple>
/*
 * This is a node for the internal graph of TrafficModel.
 * Each Node instance represents a city or crossroad.
 * It carries a Car instance from any inEdge to an outEdge of its choice (after indefinite hold).
 * It may also act as source or sink of cars on the graph.
 */
class Node {
protected:
    std::string const label;
    int id;
    float x, y;

public:
    Node(std::string label, float x, float y);
    std::string getLabel() const { return label; }
    std::vector<std::reference_wrapper<Edge>> inEdges; // ref
    std::vector<std::reference_wrapper<Edge>> outEdges;
    std::vector<std::unique_ptr<Car>> storedCars;
    virtual void collectCars() = 0;
    virtual void distributeCars() = 0;
    virtual void step(float dt) = 0;
    int getID() const { return id; }
    void setID(int id) { this->id = id; }
    std::tuple<float, float> getPosition() const { return std::make_tuple(x, y); }
    int getNCars() const { return storedCars.size(); }
    virtual std::string toString() const = 0;
};

#endif
