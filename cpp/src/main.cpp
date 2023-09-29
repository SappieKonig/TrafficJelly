#include <fstream>
#include <iostream>

#include "traffic_model.h"

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

    float dt = 0.1f;
    TrafficModelFileDirector director("tests/input/load_test.txt");
    TrafficModel trafficModel = director.build();
    trafficModel.showRoutes();
    trafficModel.display();
    trafficModel.spawnAt("A");
    trafficModel.step(dt);
    trafficModel.step(dt);
    trafficModel.visualize();
    return 0;
}
