#ifndef EDGE_H
#define EDGE_H

#include <deque>
#include <list>
#include <memory>
#include <vector>
#include <string>

#include "utils.h"
#include "observation.h"
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
    std::deque<std::unique_ptr<Car>> cars;

    Node& inNode;
    Node& outNode;
    std::string const label;

public:
    Edge(Node& inNode, Node& outNode, std::string label);
    virtual ~Edge();
    virtual void initialize() = 0;
    virtual void step(float dt, std::shared_ptr<Visualizer> visualizer) = 0; // To update the car positions in the edge. The transfer between edges is left to the model.
    virtual std::string toString() const = 0;
    virtual void enterCar(std::unique_ptr<Car>&& car) = 0;
    std::string getLabel() const;
};

class BasicRoad : public Edge
{
private:
    float const length = 0;

public:
    BasicRoad(Node& inNode, Node& outNode, std::string label, float length);
    void initialize() override;
    void step(float dt, std::shared_ptr<Visualizer> visualizer) override;
    std::string toString() const override;
    void enterCar(std::unique_ptr<Car>&& car) override;

};

#endif
