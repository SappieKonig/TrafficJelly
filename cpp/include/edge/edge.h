#ifndef EDGE_H
#define EDGE_H

#include <deque>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "utils.h"
//#include "observation.h"
#include "visualizer.h"

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

    Node& inNode;
    Node& outNode;
    std::string const label;

public:
    Edge(Node& inNode, Node& outNode, std::string label);
    virtual ~Edge();
    virtual void initialize() = 0;
    virtual void setActions() = 0;
    virtual std::string toString() const = 0;
    virtual void enterCar(std::unique_ptr<Car>&& car) = 0;
    void sortCars();
    void updateCars(float dt);
    void step(float dt) {
        setActions();
        updateCars(dt);
        sortCars();
    }
    std::string getLabel() const;
};



#endif
