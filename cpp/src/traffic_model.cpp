#include "traffic_model.h"
#include "visualizer.h"
#include "car.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

TrafficModel::TrafficModel()
    : visualizer(std::make_shared<BasicVisualizer>())
{

}

void TrafficModel::step(float dt)
{
    visualizer->reset();
    for (auto& node : nodes)
    {
        node->step(dt, visualizer);
    }

    for (auto& edge : edges)
    {
        edge->step(dt, visualizer);
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

void TrafficModel::showRoutes()
{
    for (int i = 0; i < routes.size(); i++)
    {
        std::cout << "Route " << i+1 << ":\n";
        routes[i].show();
    }
}

void TrafficModel::spawnAt(std::string label)
{
    labelToNode[label]->spawnCar();
}

void TrafficModel::visualize()
{
    std::dynamic_pointer_cast<BasicVisualizer>(visualizer)->render();
}

void TrafficModelBuilder::addBasicCity(std::string label)
{
    std::shared_ptr<Node> basicCity = std::make_shared<BasicCity>(label, trafficModel.routes, device);
    trafficModel.nodes.emplace_back(basicCity);
    trafficModel.labelToNode.emplace(label, basicCity);
}

void TrafficModelBuilder::addBasicRoad(std::string label, std::string inNodeLabel, std::string outNodeLabel, float length)
{
    std::shared_ptr<Edge> basicRoad = std::make_shared<BasicRoad>(*trafficModel.labelToNode[inNodeLabel], *trafficModel.labelToNode[outNodeLabel], label, length);
    trafficModel.edges.emplace_back(basicRoad);
    trafficModel.labelToEdge.emplace(label, basicRoad);
}

void TrafficModelBuilder::addRoute(std::vector<std::string>& nodesAlongRoute, std::vector<std::string>& edgesAlongRoute, std::vector<float>& waitingTimesAlongRoute)
{
    Route route;
    int routeLength = edgesAlongRoute.size();
    for (int i = 0; i < routeLength; i++)
    {
        route.checkpoints.emplace_back(*trafficModel.labelToNode[nodesAlongRoute[i]], *trafficModel.labelToEdge[edgesAlongRoute[i]], waitingTimesAlongRoute[i]);
    }
    trafficModel.routes.push_back(std::move(route));
}

void TrafficModelBuilder::addRoute(std::vector<std::string>&& nodesAlongRoute, std::vector<std::string>&& edgesAlongRoute, std::vector<float>&& waitingTimesAlongRoute)
{
    Route route;
    int routeLength = edgesAlongRoute.size();
    for (int i = 0; i < routeLength; i++)
    {
        route.checkpoints.emplace_back(*trafficModel.labelToNode[nodesAlongRoute[i]], *trafficModel.labelToEdge[edgesAlongRoute[i]], waitingTimesAlongRoute[i]);
    }
    trafficModel.routes.push_back(std::move(route));
}

TrafficModel& TrafficModelBuilder::build()
{
    // Initialize car (cresd naar kenzoooooooooooooooooooooooooo>
    for (std::shared_ptr<Node> node : trafficModel.nodes)
    {
        node->initialize();
    }
    for (std::shared_ptr<Edge> edge : trafficModel.edges)
    {
        edge->initialize();
    }
    return trafficModel;
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
    director.setWaitingTimesAlongRoute(args);
}

RouteNodesStringCommand::RouteNodesStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteNodesStringCommand::apply(std::vector<std::string>& args) const
{
    director.setNodesAlongRoute(args);
}

RouteEdgesStringCommand::RouteEdgesStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteEdgesStringCommand::apply(std::vector<std::string>& args) const
{
    director.setEdgesAlongRoute(args);
}

RouteStringCommand::RouteStringCommand(TrafficModelStringDirector& director)
    : StringCommand(director)
{

}

void RouteStringCommand::apply(std::vector<std::string>& args) const
{
    director.addRoute();
}

TrafficModelStringDirector::TrafficModelStringDirector(std::string str)
    : str(str)
{
    commander.emplace("BasicCity", std::unique_ptr<StringCommand>(new BasicCityStringCommand(*this)));
    commander.emplace("BasicRoad", std::unique_ptr<StringCommand>(new BasicRoadStringCommand(*this)));
    commander.emplace("RouteTimes", std::unique_ptr<StringCommand>(new RouteTimesStringCommand(*this)));
    commander.emplace("RouteNodes", std::unique_ptr<StringCommand>(new RouteNodesStringCommand(*this)));
    commander.emplace("RouteEdges", std::unique_ptr<StringCommand>(new RouteEdgesStringCommand(*this)));
    commander.emplace("Route", std::unique_ptr<StringCommand>(new RouteStringCommand(*this)));
}

TrafficModel& TrafficModelStringDirector::build()
{
    std::string buffer;
    std::string commandBuffer;
    std::vector<std::string> argumentBuffer;

    for (char ch : str)
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
    for (auto const& route : trafficModel.routes)
    {
        str += route.toString() + "\n";
    }
    std::ofstream file(fn);
    file << str;
}

void TrafficModelStringDirector::addBasicCity(std::vector<std::string>& args)
{
    trafficModelBuilder.addBasicCity(args[0]);
}

void TrafficModelStringDirector::addBasicRoad(std::vector<std::string>& args)
{
    trafficModelBuilder.addBasicRoad(args[0], args[1], args[2], std::stof(args[3]));
}

void TrafficModelStringDirector::setWaitingTimesAlongRoute(std::vector<std::string>& args)
{
    for (std::string arg : args)
    {
        waitingTimesAlongRoute.push_back(std::stof(arg));
    }
}

void TrafficModelStringDirector::setNodesAlongRoute(std::vector<std::string>& args)
{
    nodesAlongRoute = args;
}

void TrafficModelStringDirector::setEdgesAlongRoute(std::vector<std::string>& args)
{
    edgesAlongRoute = args;
}

void TrafficModelStringDirector::addRoute()
{
    trafficModelBuilder.addRoute(nodesAlongRoute, edgesAlongRoute, waitingTimesAlongRoute);
}

TrafficModelFileDirector::TrafficModelFileDirector(std::string fn)
    : fn(fn)
{

}

TrafficModel& TrafficModelFileDirector::build()
{
    std::ifstream file(fn);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    return TrafficModelStringDirector(str).build();
}

