#include <fstream>
#include <iostream>
#include <random>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    std::shared_ptr<std::random_device> device = std::make_shared<std::random_device>();
    //TrafficModelFileDirector director("tests/input/load_test.txt");
    TrafficModelFileDirector director(device, "tests/output/save_test.txt");
    TrafficModel trafficModel = director.build();
    trafficModel.display();
    trafficModel.showRoutes();
    return 0;
}
