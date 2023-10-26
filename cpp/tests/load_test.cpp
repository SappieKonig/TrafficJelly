#include <fstream>
#include <iostream>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    //TrafficModelFileDirector director("tests/input/load_test.txt");
    TrafficModelFileDirector director("tests/output/save_test.txt");
    TrafficModel trafficModel = director.build();
    trafficModel.display();
    trafficModel.showRoutes();
    return 0;
}
