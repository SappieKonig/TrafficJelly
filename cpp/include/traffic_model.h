#ifndef WORLD_H
#define WORLD_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <optional>
#include <memory>
#include <any>

#include "utils.h"
#include "car.h"
#include "node/node.h"
#include "edge/edge.h"
#include "visualizer.h"
#include "route.h"

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
    std::vector<Route> routes;
    // Convenient utility for users
    std::unordered_map<std::string, std::shared_ptr<Node>> labelToNode;
    std::unordered_map<std::string, std::shared_ptr<Edge>> labelToEdge;

    std::shared_ptr<Visualizer> visualizer; // The visualiser is shared pointer and not unique to enable downcasts. We currently only use basic visualization.
    std::vector<std::shared_ptr<Edge>> edges;

    TrafficModel();

public:
    TrafficModel(std::string fn, float delta_time);
    // Model usage and interpretation
    void step(float dt);
    void transferCars();
    void display() const; // Only reasonably used, if small graph
    void showRoutes() const;
    // getFirstEdge
    Edge& getEdge(int idx) {
        return *edges[idx];
    }
    friend TrafficModelBuilder;
    void setIDs();
    std::vector<int> getEdgeIDs();
    std::vector<int> getNodeIDs();
    float getEdgeRoadLength(int idx) {
        return edges[idx]->getLength();
    }
    int getEdgeStartNodeID(int idx) {
        return edges[idx]->getInNode().getID();
    }
    int getEdgeEndNodeID(int idx) {
        return edges[idx]->getOutNode().getID();
    }
    std::tuple<float, float> getNodePosition(int idx) {
        return nodes[idx]->getPosition();
    }
    std::tuple<std::vector<int>, std::vector<float>> getCarCountHistInEdge(int idx, float bin_distance) {
        return edges[idx]->getCarCountHist(bin_distance);
    }
    int getCarCountInNode(int idx) {
        return nodes[idx]->getNCars();
    }
};

/*
 * This traffic model builder allows for the traffic model to be constructed incrementally before usage.
 * This may be enhanced to capture more complicated traffic constructions.
 * After build the traffic model is empty, so the builder could be used more than once.
 */
class TrafficModelBuilder
{
private:
    TrafficModel trafficModel;
    std::random_device device;

public:
    void addBasicCity(std::string label, int population);
    void addBasicRoad(std::string label, std::string inNodeLabel, std::string outNodeLabel, float length, float speedLimit, int nLanes);
    void addRoute(std::vector<std::string> nodesAlongRoute, std::vector<std::string> edgesAlongRoute, std::vector<float> waitingTimesAlongRoute);
    TrafficModel build();
    void save(std::string fn) const;
};

class TrafficModelStringDirector;

/*
 * This string command abstract base class represents commands that are called during the string directed build process.
 */
class StringCommand
{
protected:
    TrafficModelStringDirector& director;

public:
    StringCommand(TrafficModelStringDirector& director);
    virtual void apply(std::vector<std::string>& args) const = 0;
};

/*
 * This string command builds a basic city.
 */
class BasicCityStringCommand : public StringCommand
{
public:
    BasicCityStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This string command builds a basic road.
 */
class BasicRoadStringCommand : public StringCommand
{
public:
    BasicRoadStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This string command caches waiting times.
 */
class RouteTimesStringCommand : public StringCommand
{
public:
    RouteTimesStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This string command caches nodes.
 */
class RouteNodesStringCommand : public StringCommand
{
public:
    RouteNodesStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This traffic model builder allows for the traffic model to be constructed incrementally before usage.
 * This may be enhanced to capture more complicated traffic constructions.
 * After build the traffic model is empty, so the builder could be used more than once.
 */
class RouteEdgesStringCommand : public StringCommand
{
public:
    RouteEdgesStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This string command adds the final route.
 */
class RouteStringCommand : public StringCommand
{
public:
    RouteStringCommand(TrafficModelStringDirector& director);
    void apply(std::vector<std::string>& args) const override;
};

/*
 * This traffic model string director allows for the traffic model to be constructed from a string.
 */
class TrafficModelStringDirector
{
private:
    std::string file_content;
    std::unordered_map<std::string, std::unique_ptr<StringCommand>> commander;
    TrafficModelBuilder trafficModelBuilder;
    std::vector<std::string> nodesAlongRoute;
    std::vector<std::string> edgesAlongRoute;
    std::vector<float> waitingTimesAlongRoute;

public:
    TrafficModelStringDirector(std::string fn);
    TrafficModel build();
    void addBasicCity(std::vector<std::string>& args); // Not safe, if existent label. Also, probably should add argument object for more clarity of the parameters in code. Is getting tedious though.
                                                       // TODO: Add x, y coordinates
    void addBasicRoad(std::vector<std::string>& args);
    void addNodesAlongRoute(std::vector<std::string> args);
    void addEdgesAlongRoute(std::vector<std::string> args);
    void addWaitingTimesAlongRoute(std::vector<std::string>& args);
    void addRoute();

    // Add string commands as friends, because I am too stupid to solve the encapsulation issues.
    friend BasicRoadStringCommand;
    friend BasicCityStringCommand;
};

/*
 * This traffic model file director allows for the traffic model to be constructed from a text file.
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
