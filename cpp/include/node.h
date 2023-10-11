#ifndef NODE_H
#define NODE_H

#include <list>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "utils.h"
#include "car.h"
#include "edge/edge.h"
#include "visualizer.h"

/*
 * This is a node for the internal graph of TrafficModel.
 * Each Node instance represents a city or crossroad.
 * It carries a Car instance from any inEdge to an outEdge of its choice (after indefinite hold).
 * It may also act as source or sink of cars on the graph.
 */

class Node {
private:
    std::string const label;

public:
    Node(std::string label);
    std::vector<std::reference_wrapper<Edge>> inEdges; // ref
    std::vector<std::reference_wrapper<Edge>> outEdges;
    std::string getLabel() { std::cout << "whoops" << std::endl; return label; }
    void step(float dt) {
        for (auto& edge : inEdges) {
            edge.get().step(dt);
        }
    }
};

//class Node
//{
//protected:
//    // To keep track of the cars present in the node.
//    std::list<std::unique_ptr<Car>> cars;
//
//    std::vector<std::reference_wrapper<Edge>> inEdges; // ref
//    std::vector<std::reference_wrapper<Edge>> outEdges;
//    std::string const label;
//
//public:
//    Node(std::string label);
//    virtual ~Node();
//    virtual void initialize() = 0;
//    virtual void step(float dt, std::shared_ptr<Visualizer> visualizer) = 0; // To update the car positions, or durations before transferring
//    virtual void spawnCar() = 0; // Does not make sense for non-cities
//    virtual std::string toString() const = 0;
//    std::string getLabel() const;
//    void enterCar(std::unique_ptr<Car>&& car);
//
//    friend Edge::Edge(Node& inNode, Node& outNode, std::string label); // Ugly, I know
//};
//
///*
// * This is a basic city node that only holds and transfers nodes.
// */
//class BasicCity : public Node
//{
////private:
////    BasicCarFactory carFactory; // Factories do not exist in crossroads, so it makes sense to store the factory locally.
//
//public:
//    BasicCity(std::string label, std::vector<Route>& routes, std::random_device& device);
//    void initialize() override;
//    void step(float dt, std::shared_ptr<Visualizer> visualizer) override;
//    void spawnCar() override;
//    std::string toString() const override;
//};

#endif
