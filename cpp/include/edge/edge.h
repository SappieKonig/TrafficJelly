#ifndef EDGE_H
#define EDGE_H

#include <deque>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include "utils.h"
#include "car.h"

/*
 * This is an edge for the internal graph of TrafficModel.
 * Each Edge instance represents a road with cars driving on it.
 * It carries any Car instance from inNode to outNode.
 */
class Edge
{
protected:
    // To keep track of every car in the driving order.
    std::list<std::unique_ptr<Car>> cars;
    float const speedLimit; // in m/s
    int id;

    Node& inNode;
    std::string const label;
public:
    float length; // In meters

    Node& outNode;
    Edge(Node& inNode, Node& outNode, std::string label, float speedLimit);
    virtual ~Edge();
    virtual void setActions() = 0;
    virtual void enterCar(std::unique_ptr<Car>&& car) = 0;
    std::vector<std::unique_ptr<Car>> getExitingCars();
    void sortCars();
    void updateCars(float dt);
    void step(float dt) {
        setActions();
        updateCars(dt);
        sortCars();
    }
    std::string getLabel() const;
    int getNCars() const { return cars.size(); }
    int getID() const { return id; }
    void setID(int id) { this->id = id; }
    float getLength() const { return length; }
    Node& getInNode() const { return inNode; }
    Node& getOutNode() const { return outNode; }
    std::tuple<std::vector<int>, std::vector<float>> getCarCountHist(float bin_distance) const;
    float getExpectedCrossingTime() const { return length / speedLimit; }
};



#endif
