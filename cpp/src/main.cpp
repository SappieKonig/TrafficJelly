#include <fstream>
#include <iostream>
#include <memory>

#include "basic_road.h"
#include "traffic_model.h"
#include "node.h"
#include "car.h"

int main()
{
//  std::string routes = "a";
//  std::string d;
//  struct B
//  {
//      std::string& routes;
//      B(std::string& a) : routes(a) {}
//  };
//  B b(routes);
//  d = routes;
//  routes += "x";
//  std::cout << b.routes << "\n";

    float dt = 0.2f;
//    Node inNode = {"inNode"};
//    Node outNode = {"outNode"};
//    BasicRoad road = {inNode, outNode, "road", 1000, 1};

    std::shared_ptr<std::random_device> device = std::make_shared<std::random_device>();
    TrafficModelFileDirector director(device, "tests/output/save_test.txt");
    TrafficModel trafficModel = director.build();
    auto& firstEdge1 = trafficModel.getEdge("A2-1");
    auto& firstEdge2 = trafficModel.getEdge("A2-2");
    auto& secondEdge1 = trafficModel.getEdge("A4-1");
    auto& secondEdge2 = trafficModel.getEdge("A4-2");
    auto& thirdEdge1 = trafficModel.getEdge("A12-1");
    auto& thirdEdge2 = trafficModel.getEdge("A12-2");
    trafficModel.spawnCar("Utrecht");
    for (int i = 0; i < 10000; i++)
    {
        trafficModel.step(dt);
        trafficModel.transferCars();
        std::cout << firstEdge2.getNCars() << ", " << secondEdge1.getNCars() << ", " << thirdEdge1.getNCars() << "\n";
    }
    trafficModel.showRoutes();
    trafficModel.display();
//    trafficModel.spawnAt("A");
//    trafficModel.step(dt);
//    trafficModel.step(dt);
//    trafficModel.visualize();
//    return 0;
}
