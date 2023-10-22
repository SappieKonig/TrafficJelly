#include "traffic_model.h"
#include <pybind11/pybind11.h>
//#include "visualizer.h"
#include "car.h"
#include "edge/basic_road/basic_road.h"
#include "node/basic_city.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

TrafficModel::TrafficModel(std::string fn)
{
    std::ifstream file(fn);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    auto builder = TrafficModelBuilder(*this);
    builder.build(str);
}

void TrafficModel::step(float dt)
{
    for (auto& node : nodes)
    {
        node->step(dt);
    }
    for (auto& edge : edges)
    {
        edge->step(dt);
    }
}

void TrafficModel::transferCars() {
    for (auto& node : nodes)
    {
        node->collectCars();
        node->distributeCars();
    }
}

void TrafficModel::display() const
{
    std::cout << "Nodes:\n";
    for (auto& node : nodes)
    {
        std::cout << "-" << node->getLabel() << "\n";
    }

    std::cout << "Edges:\n";
    for (auto& edge : edges)
    {
        std::cout << "-" << edge->getLabel() << "\n";
    }
}

void TrafficModelBuilder::addBasicCity(std::string label, int population) {
    std::shared_ptr<Node> node = std::make_shared<BasicCity>(label, population);
    trafficModel.nodes.emplace_back(node);
    trafficModel.labelToNode[label] = node;
}

void TrafficModelBuilder::addBasicRoad(std::string label, std::string inNodeLabel, std::string outNodeLabel, float length, float speedLimit, int nLanes)
{
    std::shared_ptr<Node> inNode = trafficModel.labelToNode[inNodeLabel];
    std::shared_ptr<Node> outNode = trafficModel.labelToNode[outNodeLabel];
    std::shared_ptr<Edge> edge = std::make_shared<BasicRoad>(*inNode, *outNode, label, length, speedLimit, nLanes);
    trafficModel.edges.emplace_back(edge);
    trafficModel.labelToEdge[label] = edge;
}

StringCommand::StringCommand(TrafficModelBuilder& trafficModelBuilder)
    : trafficModelBuilder(trafficModelBuilder) {}


BasicCityStringCommand::BasicCityStringCommand(TrafficModelBuilder& trafficModelBuilder)
    : StringCommand(trafficModelBuilder) {}


void BasicCityStringCommand::apply(std::vector<std::string>& args) const
{
    trafficModelBuilder.addBasicCity(args[0], std::stoi(args[1]));
}

BasicRoadStringCommand::BasicRoadStringCommand(TrafficModelBuilder& trafficModelBuilder)
    : StringCommand(trafficModelBuilder) {}

void BasicRoadStringCommand::apply(std::vector<std::string>& args) const
{
    trafficModelBuilder.addBasicRoad(args[0], args[1], args[2], std::stof(args[3]), std::stof(args[4]), std::stoi(args[5]));
}

TrafficModelBuilder::TrafficModelBuilder(TrafficModel& trafficModel)
    : trafficModel(trafficModel)
{
    commander.emplace("BasicRoad", std::unique_ptr<StringCommand>(new BasicRoadStringCommand(*this)));
    commander.emplace("BasicCity", std::unique_ptr<StringCommand>(new BasicCityStringCommand(*this)));
}

void TrafficModelBuilder::build(std::string file_content)
{
    std::string buffer;
    std::string commandBuffer;
    std::vector<std::string> argumentBuffer;
    for (char ch : file_content)
    {
        switch (ch)
        {
            case ':':
                commandBuffer = buffer;
                buffer.clear();
                break;
            case ',':
                argumentBuffer.push_back(buffer);
                buffer.clear();
                break;
            case '\n':
                argumentBuffer.push_back(buffer);

                commander[commandBuffer]->apply(argumentBuffer);

                commandBuffer.clear();
                argumentBuffer.clear();
                buffer.clear();
                break;
            default:
                buffer.push_back(ch);
                break;
        }
    }
}


PYBIND11_MODULE(traffic_model, m) {
    pybind11::class_<TrafficModel>(m, "TrafficModel")
        .def(pybind11::init<const std::string &>())
        .def("step", &TrafficModel::step)
        .def("display", &TrafficModel::display);
}

