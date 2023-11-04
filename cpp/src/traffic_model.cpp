#include "traffic_model.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "edge/basic_road/basic_road.h"
#include "node/basic_city.h"
#include "route.h"


#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

TrafficModel::TrafficModel(std::string fn, float delta_time, float scale)
    : delta_time(delta_time), population(0), global_time(0), scale(scale)
{
    std::ifstream file(fn);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    auto builder = TrafficModelBuilder(*this);
    builder.build(str);
    std::cout << "TrafficModel built.\n";
    std::cout << "Nodes: " << nodes.size() << "\n";
    std::cout << "Edges: " << edges.size() << "\n";
    setIDs();
    shortestPathMapping = computeMapping(nodes);
    std::vector<int> populations;
    populations.reserve(nodes.size());
    for (auto& node : nodes) {
        populations.push_back(node->population * scale);
        node->x *= scale;
        node->y *= scale;
    }
    population = std::accumulate(populations.begin(), populations.end(), 0);
    mappingProbabilities = computeProbabilities(populations);
    generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
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
    spawnCars();
    transferCars();
    global_time += delta_time / scale;
}

void TrafficModel::spawnCar() {
    float p = distribution(generator);
    for (int i = 0; i < mappingProbabilities.size(); i++) {
        for (int j = 0; j < mappingProbabilities[i].size(); j++) {
            p -= mappingProbabilities[i][j];
            if (p < 0) {
                auto path = getFastestPath(i, j);
                nodes[i]->spawnCar(path, global_time);
                return;
            }
        }
    }
}


void TrafficModel::spawnCars() {
    // Calculate spawns
    float spawn_prob = (float) population * 0.2f / 86400.0f * delta_time;
//    if (global_time == 0) {
//        spawn_prob = 1;
//    } else {
//        spawn_prob = 0;
//    }
    // Multiply spawn_prob by 4 if between 7 and 9 and 16 and 18
    int hour = (int) global_time % 86400 / 3600;
    if ((hour >= 7 && hour < 9) || (hour >= 16 && hour < 18)) {
        spawn_prob *= 4;
    }
    while (spawn_prob > 1) {
        spawnCar();
        spawn_prob -= 1;
    }
    if (distribution(generator) < spawn_prob) {
        spawnCar();
    }
}

void TrafficModel::setIDs() {
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

void TrafficModelBuilder::addBasicCity(std::string label, int population, float x, float y) {
    std::shared_ptr<Node> node = std::make_shared<BasicCity>(label, population, x, y);
    trafficModel.nodes.emplace_back(node);
    trafficModel.labelToNode[label] = node;
}

void TrafficModelBuilder::addBasicRoad(std::string label, std::string inNodeLabel, std::string outNodeLabel, float speedLimit, int nLanes)
{
    std::shared_ptr<Node> inNode = trafficModel.labelToNode[inNodeLabel];
    std::shared_ptr<Node> outNode = trafficModel.labelToNode[outNodeLabel];
    std::shared_ptr<Edge> edge = std::make_shared<BasicRoad>(*inNode, *outNode, label, speedLimit, nLanes);
    trafficModel.edges.emplace_back(edge);
    trafficModel.labelToEdge[label] = edge;
}

StringCommand::StringCommand(TrafficModelBuilder& trafficModelBuilder)
    : trafficModelBuilder(trafficModelBuilder) {}


BasicCityStringCommand::BasicCityStringCommand(TrafficModelBuilder& trafficModelBuilder)
    : StringCommand(trafficModelBuilder) {}


void BasicCityStringCommand::apply(std::vector<std::string>& args) const
{
    trafficModelBuilder.addBasicCity(args[0], std::stoi(args[1]), std::stof(args[2]), std::stof(args[3]));
}

BasicRoadStringCommand::BasicRoadStringCommand(TrafficModelBuilder& trafficModelBuilder)
    : StringCommand(trafficModelBuilder) {}

void BasicRoadStringCommand::apply(std::vector<std::string>& args) const
{
    trafficModelBuilder.addBasicRoad(args[0], args[1], args[2], std::stof(args[3]), std::stoi(args[4]));
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

int TrafficModel::getNCarsInSimulation() {
    int n = 0;
    for (auto& node : nodes) {
        n += node->getNCars();
    }
    for (auto& edge : edges) {
        n += edge->getNCars();
    }
    return n;
}


PYBIND11_MODULE(traffic_model, m) {
    pybind11::class_<TrafficModel>(m, "TrafficModel")
        .def(pybind11::init<const std::string &, float, float>())
        .def("step_forward", &TrafficModel::step)
        .def("display", &TrafficModel::display)
        .def("get_edge_ids", &TrafficModel::getEdgeIDs)
        .def("get_node_ids", &TrafficModel::getNodeIDs)
        .def("get_edge_road_length", &TrafficModel::getEdgeRoadLength)
        .def("get_edge_start_node_id", &TrafficModel::getEdgeStartNodeID)
        .def("get_edge_end_node_id", &TrafficModel::getEdgeEndNodeID)
        .def("get_node_pos", &TrafficModel::getNodePosition)
        .def("get_car_count_histogram_in_edge", &TrafficModel::getCarCountHistInEdge)
        .def("get_car_count_in_node", &TrafficModel::getCarCountInNode)
        .def("get_fastest_path", &TrafficModel::getFastestPath)
        .def("get_delta_time", &TrafficModel::getDeltaTime)
        .def("get_n_cars_in_simulation", &TrafficModel::getNCarsInSimulation)
        .def("get_n_cars_per_edge", &TrafficModel::getNCarsPerEdge)
        .def("get_travel_stats", &TrafficModel::getTravelStats);
}

