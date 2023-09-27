#include "traffic_model.h"

#include <iostream>
#include <memory>

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

void TrafficModel::display() const
{
    std::cout << "Nodes:\n";
    for (auto& node : nodes)
    {
        node->getLabel();
    }

    std::cout << "Edges:\n";
    for (auto& edge : edges)
    {
        edge->getLabel();
    }
}

void TrafficModelBuilder::addBasicCity(std::string label)
{
    trafficModel.nodes.emplace_back(new BasicCity(label));
    trafficModel.labelToNode.emplace(label, trafficModel.nodes[-1]);
}

void TrafficModelBuilder::addBasicRoad(std::string inNodeLabel, std::string outNodeLabel, std::string label, float length)
{
    trafficModel.edges.emplace_back(new BasicRoad(*trafficModel.labelToNode[inNodeLabel], *trafficModel.labelToNode[outNodeLabel], label, length));
}

TrafficModelStringDirector::TrafficModelStringDirector(std::string str)
    : str(str)
{
    commander.emplace("BasicCity", &TrafficModelStringDirector::addBasicCity);
    commander.emplace("BasicRoad", &TrafficModelStringDirector::addBasicRoad);
}

TrafficModel TrafficModelStringDirector::build()
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

                commander[commandBuffer](argumentBuffer);

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
    std::ofstream file(fn);
    file << str;
}

void TrafficModelStringDirector::addBasicCity(std::vector<std::string>& args)
{
    trafficModelBuilder.addBasicCity(args[0]);
}

void TrafficModelStringDirector::addBasicRoad(std::vector<std::string>& args)
{
    trafficModelBuilder.addBasicRoad(args[0], args[1], args[2], std::stod(args[2]));
}

TrafficModelFileDirector::TrafficModelFileDirector(std::string fn)
    : fn(fn)
{

}

TrafficModel TrafficModelFileDirector::build()
{
    std::ifstream file(fn);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    std::cout << str << "\n";
    return TrafficModelStringDirector(str).build();
}

