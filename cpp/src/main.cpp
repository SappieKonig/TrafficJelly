#include <fstream>
#include <iostream>

#include "edge/basic_road/basic_road.h"
#include "traffic_model.h"
#include "node/node.h"
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

    TrafficModelFileDirector director("../tests/input/load_test.txt");
    TrafficModel trafficModel = director.build();
    std::unique_ptr<Car> car = std::make_unique<Car>();
    auto& firstEdge = trafficModel.getEdge(0);
    auto& secondEdge = trafficModel.getEdge(1);
    auto& thirdEdge = trafficModel.getEdge(2);
    firstEdge.enterCar(std::move(car));
    for (int i = 0; i < 10000; i++)
    {
        trafficModel.step(dt);
        trafficModel.transferCars();
        std::cout << firstEdge.getNCars() << ", " << secondEdge.getNCars() << ", " << thirdEdge.getNCars() << "\n";
    }
//    trafficModel.showRoutes();
    trafficModel.display();
//    trafficModel.spawnAt("A");
//    trafficModel.step(dt);
//    trafficModel.step(dt);
//    trafficModel.visualize();
//    return 0;
}
