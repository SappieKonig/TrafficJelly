#ifndef WORLD_H
#define WORLD_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <optional>
#include <memory>

#include "utils.h"
#include "car.h"
#include "node.h"
#include "edge.h"

class TrafficModelBuilder;

/*
 * This is a traffic model for cars moving on a graph.
 * Roads are represented by edges, and cities or crossroads by nodes.
 * The internal graph may be loaded directly after creation from a text file given in the correct format.
 * To simulate, step(dt) may be called to move forward in time by a step dt.
 */
class TrafficModel
{
private:
    // Holds all information of the model.
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    std::vector<Route> routes;

    // Convenient utility for users
    std::unordered_map<std::string, std::shared_ptr<Node>> labelToNode;

    TrafficModel();

public:
    // Model usage and interpretation
    void step(float dt);
    void display() const; // Only reasonably used, if small graph

    friend TrafficModelBuilder;
};

/*
 * This traffic model builder allows for the traffic model to be constructed incrementally before usage.
 * This may be enhanced to capture more complicated traffic constructions.
 */
class TrafficModelBuilder
{
private:
    TrafficModel trafficModel;

public:
    void addBasicCity(std::string label); // Not safe, if existent label
    void addBasicRoad(std::string inNodeLabel, std::string outNodeLabel, std::string label, float length);
    TrafficModel build();

    void save(std::string fn) const;
};

/*
 * This traffic model string director allows for the traffic model to be constructed from a text file.
 */
class TrafficModelStringDirector
{
private:
    std::string str;
    std::unordered_map<std::string, std::function<void(std::reference_wrapper<std::vector<std::string>>)>> commander;
    TrafficModelBuilder trafficModelBuilder;

public:
    TrafficModelStringDirector(std::string fn);
    TrafficModel build();
    void addBasicCity(std::vector<std::string>& args); // Not safe, if existent label
    void addBasicRoad(std::vector<std::string>& args);
};

/*
 * This traffic model file director allows for the traffic model to be constructed from a string.
 */
class TrafficModelFileDirector
{
private:
    std::string fn;

public:
    TrafficModelFileDirector(std::string fn);
    TrafficModel build();
};

#endif
