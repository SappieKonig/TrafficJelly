#include "traffic_model.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//#include "visualizer.h"
#include "car.h"
#include "basic_road.h"
#include "basic_city.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <random>
#include <vector>

TrafficModel::TrafficModel(std::shared_ptr<std::random_device> device)
    : device(device)
{
    setIDs();
    routes->reserve(1);
}

void TrafficModel::step()
{
    for (auto& edge : edges)
    {
        edge->step(delta_time);
    }
    for (auto& node : nodes)
    {
        node->step(delta_time);
    }
    transferCars();
}

void TrafficModel::setIDs()
{
    int id = 0;
    for (auto& node : nodes)
    {
        node->setID(id++);
    }
    id = 0;
    for (auto& edge : edges)
    {
        edge->setID(id++);
    }
}

std::vector<int> TrafficModel::getEdgeIDs() {
    std::vector<int> ids;
    for (auto& edge : edges)
    {
        ids.push_back(edge->getID());
    }
    return ids;
}

std::vector<int> TrafficModel::getNodeIDs() {
    std::vector<int> ids;
    for (auto& node : nodes)
    {
        ids.push_back(node->getID());
    }
    return ids;
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

void TrafficModel::showRoutes() const
{
    for (Route const& route : *routes)
    {
        route.show();
    }
}

void TrafficModel::spawnCar(std::string label)
{
    std::dynamic_pointer_cast<BasicCity>(labelToNode[label])->spawnCar();
}

TrafficModelBuilder::TrafficModelBuilder(std::shared_ptr<std::random_device> device)
    : trafficModel(device), device(device)
{

}

void TrafficModelBuilder::addBasicCity(std::string label, int population, float x, float y) {
    std::shared_ptr<Node> node = std::make_shared<BasicCity>(label, population, x, y trafficModel.routes, device);
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


void TrafficModelBuilder::addRoute(std::vector<std::string> nodesAlongRoute, std::vector<std::string> edgesAlongRoute, std::vector<float> waitingTimesAlongRoute)
{
    Route route;
    for (int i = 0; i < nodesAlongRoute.size(); i++)
    {
        Checkpoint checkpoint(trafficModel.labelToNode[nodesAlongRoute[i]], trafficModel.labelToEdge[edgesAlongRoute[i]], waitingTimesAlongRoute[i]);
        route.checkpoints.push_back(checkpoint);
    }
    trafficModel.routes->push_back(route);
}

StringCommand::StringCommand(TrafficModelStringDirector& director)
    : director(director)
{

}

BasicCityStringCommand::BasicCityStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void BasicCityStringCommand::apply(std::vector<std::string>& args) const
{
    director.addBasicCity(args);
}

BasicRoadStringCommand::BasicRoadStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void BasicRoadStringCommand::apply(std::vector<std::string>& args) const
{
    director.addBasicRoad(args);
}

RouteTimesStringCommand::RouteTimesStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteTimesStringCommand::apply(std::vector<std::string>& args) const
{
    director.addWaitingTimesAlongRoute(args);
}

RouteNodesStringCommand::RouteNodesStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteNodesStringCommand::apply(std::vector<std::string>& args) const
{
    director.addNodesAlongRoute(args);
}

RouteEdgesStringCommand::RouteEdgesStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteEdgesStringCommand::apply(std::vector<std::string>& args) const
{
    director.addEdgesAlongRoute(args);
}

RouteStringCommand::RouteStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteStringCommand::apply(std::vector<std::string>& args) const
{
    director.addRoute();
}

TrafficModelStringDirector::TrafficModelStringDirector(std::shared_ptr<std::random_device> device, std::string file_content)
    : trafficModelBuilder(device), file_content(file_content)
{
    commander.emplace("BasicRoad", std::unique_ptr<StringCommand>(new BasicRoadStringCommand(*this)));
    commander.emplace("BasicCity", std::unique_ptr<StringCommand>(new BasicCityStringCommand(*this)));
    commander.emplace("RouteTimes", std::unique_ptr<StringCommand>(new RouteTimesStringCommand(*this)));
    commander.emplace("RouteNodes", std::unique_ptr<StringCommand>(new RouteNodesStringCommand(*this)));
    commander.emplace("RouteEdges", std::unique_ptr<StringCommand>(new RouteEdgesStringCommand(*this)));
    commander.emplace("Route", std::unique_ptr<StringCommand>(new RouteStringCommand(*this)));
}

TrafficModel TrafficModelStringDirector::build()
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

    return trafficModelBuilder.build();
}

TrafficModel TrafficModelBuilder::build()
{
    TrafficModel ret = std::move(trafficModel);
    trafficModel.device = device;
    return ret;
}

void TrafficModelBuilder::save(std::string fn) const
{
    std::string str = "";
    for (auto const& node : trafficModel.nodes)
    {
        str += node->toString() + "\n";
    }
    for (auto const& edge : trafficModel.edges)
    {
        str += edge->toString() + "\n";
    }
    for (auto const& route : *trafficModel.routes)
    {
        str += route.toString() + "\n";
    }
    std::ofstream file(fn);
    file << str;
}

void TrafficModelStringDirector::addBasicCity(std::vector<std::string> &args)
{
    trafficModelBuilder.addBasicCity(args[0], std::stoi(args[1]));
}

void TrafficModelStringDirector::addBasicRoad(std::vector<std::string>& args)
{
    trafficModelBuilder.addBasicRoad(args[0], args[1], args[2], std::stof(args[3]), std::stof(args[4]), std::stoi(args[5]));
}

void TrafficModelStringDirector::addNodesAlongRoute(std::vector<std::string> args)
{
    nodesAlongRoute = args;
}

void TrafficModelStringDirector::addEdgesAlongRoute(std::vector<std::string> args)
{
    edgesAlongRoute = args;
}

void TrafficModelStringDirector::addWaitingTimesAlongRoute(std::vector<std::string>& args)
{
    waitingTimesAlongRoute.clear();
    for (std::string arg : args)
    {
        waitingTimesAlongRoute.push_back(std::stof(arg));
    }
}

void TrafficModelStringDirector::addRoute()
{
    trafficModelBuilder.addRoute(nodesAlongRoute, edgesAlongRoute, waitingTimesAlongRoute);
}

TrafficModelFileDirector::TrafficModelFileDirector(std::shared_ptr<std::random_device> device, std::string fn)
    : device(device), fn(fn)
{

}

TrafficModel TrafficModelFileDirector::build()
{
    std::ifstream file(fn);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    return TrafficModelStringDirector(device, str).build();
}

PYBIND11_MODULE(traffic_model, m) {
    pybind11::class_<TrafficModel>(m, "TrafficModel")
        .def(pybind11::init<const std::string &, float>())
        .def("step", &TrafficModel::step)
        .def("display", &TrafficModel::display)
        .def("get_edge_ids", &TrafficModel::getEdgeIDs)
        .def("get_node_ids", &TrafficModel::getNodeIDs)
        .def("get_edge_road_length", &TrafficModel::getEdgeRoadLength)
        .def("get_edge_start_node_id", &TrafficModel::getEdgeStartNodeID)
        .def("get_edge_end_node_id", &TrafficModel::getEdgeEndNodeID)
        .def("get_node_pos", &TrafficModel::getNodePosition)
        .def("get_car_count_histogram_in_edge", &TrafficModel::getCarCountHistInEdge)
        .def("get_car_count_in_node", &TrafficModel::getCarCountInNode);
}
