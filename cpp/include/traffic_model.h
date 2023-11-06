#ifndef WORLD_H
#define WORLD_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <optional>
#include <memory>
#include <any>
#include <random>

#include "utils.h"
#include "car.h"
#include "node/node.h"
#include "edge/edge.h"
#include "route.h"

#define TravelStats std::tuple<int, int, float, float>

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
    std::vector<std::vector<int>> shortestPathMapping;
    float delta_time;
    std::vector<std::shared_ptr<Node>> nodes;
    // Convenient utility for users
    std::unordered_map<std::string, std::shared_ptr<Node>> labelToNode;
    std::unordered_map<std::string, std::shared_ptr<Edge>> labelToEdge;
    int population;
    std::vector<std::shared_ptr<Edge>> edges;
    float scale;
    int carID = 0;
public:
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
    std::vector<std::vector<float>> mappingProbabilities;
    TrafficModel(std::string fn, float delta_time, float scale);
    // Model usage and interpretation
    void spawnCar();
    void step();
    void transferCars();
    void display() const; // Only reasonably used, if small graph
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
    int getEdgeLaneCount(int idx) {
        return 3;
    }
    int getCarLane(int carID) {
        for (auto& edge : edges) {
            for (auto& car : edge->cars) {
                if (car->carID == carID) {
                    return car->lane;
                }
            }
        }
    }
    int getCarX(int carID) {
        for (auto& edge : edges) {
            for (auto& car : edge->cars) {
                if (car->carID == carID) {
                    return car->getX();
                }
            }
        }
    }
    std::vector<int> getCarIDsInEdgeOnInterval(int idx, std::tuple<float, float> interval) {
        return edges[idx]->getCarIDsOnInterval(interval);
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
    std::vector<int> getFastestPath(int startNodeID, int endNodeID) {
        return reconstructPath(shortestPathMapping, startNodeID, endNodeID);
    }
    float global_time;
    float getDeltaTime() const { return delta_time; }

    void spawnCars();

    std::string getLabelFromNodeID(int nodeID) {
        return nodes[nodeID]->getLabel();
    }

    std::string getLabelFromEdgeID(int edgeID) {
        return edges[edgeID]->getLabel();
    }

    int getNCarsInSimulation();

    std::vector<int> getNCarsPerEdge() {
        std::vector<int> nCarsPerEdge;
        nCarsPerEdge.reserve(edges.size());
        for (auto& edge : edges) {
            nCarsPerEdge.push_back(edge->getNCars());
        }
        return nCarsPerEdge;
    }

    std::vector<TravelStats> getTravelStats() {
        std::vector<TravelStats> travelStats;
        for (auto& node : nodes) {
            travelStats.insert(travelStats.end(), node->travelStats.begin(), node->travelStats.end());
        }
        return travelStats;
    }
};

/*
 * This string command abstract base class represents commands that are called during the string directed build process.
 */
class StringCommand
{
protected:
    TrafficModelBuilder &trafficModelBuilder;

public:
    StringCommand(TrafficModelBuilder &trafficModelBuilder);
    virtual void apply(std::vector<std::string>& args) const = 0;
};

class BasicRoadStringCommand : public StringCommand
{
public:
    BasicRoadStringCommand(TrafficModelBuilder &trafficModelBuilder);
    void apply(std::vector<std::string>& args) const override;
};

class BasicCityStringCommand : public StringCommand
{
public:
    BasicCityStringCommand(TrafficModelBuilder &trafficModelBuilder);
    void apply(std::vector<std::string>& args) const override;
};

#endif

/*
 * This traffic model builder allows for the traffic model to be constructed incrementally before usage.
 * This may be enhanced to capture more complicated traffic constructions.
 * After build the traffic model is empty, so the builder could be used more than once.
 */
class TrafficModelBuilder
{
private:
    TrafficModel &trafficModel;
    std::string file_content;
    std::unordered_map<std::string, std::unique_ptr<StringCommand>> commander;

public:
    TrafficModelBuilder(TrafficModel& trafficModel);
    void build(std::string file_content);
    void addBasicCity(std::string label, int population, float x, float y);
    void addBasicRoad(std::string label, std::string inNodeLabel, std::string outNodeLabel, float speedLimit, int nLanes);

    friend BasicRoadStringCommand;
    friend BasicCityStringCommand;
};
