#ifndef NODE_H
#define NODE_H

#include <list>
#include <vector>
#include <string>

#include "utils.h"
#include "car.h"
#include "edge.h"

/*
 * This is a node for the internal graph of TrafficModel.
 * Each Node instance represents a city or crossroad.
 * It carries a Car instance from any inEdge to an outEdge of its choice (after indefinite hold).
 * It may also act as source or sink of cars on the graph.
 */
class Node
{
protected:
    // To keep track of the cars present in the node.
    std::list<std::unique_ptr<Car>> cars;

    std::vector<std::reference_wrapper<Edge>> inEdges; // ref
    std::vector<std::reference_wrapper<Edge>> outEdges;
    std::string const label;

public:
    Node(std::string label);
    virtual ~Node() = default;
    virtual void step(float dt) = 0; // To update the car positions, or durations before transferring
    virtual std::string toString() const = 0;
    std::string getLabel() const;
    void addCar(std::unique_ptr<Car>&& car);

    friend Edge::Edge(Node& inNode, Node& outNode, std::string label); // Ugly, I know
};

class BasicCity : public Node
{
public:
    BasicCity(std::string label);
    void step(float dt) override;
    std::string toString() const override;
};

#endif
