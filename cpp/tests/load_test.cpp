#include <fstream>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    TrafficModelFileDirector director("tests/input/load_test.txt");
    TrafficModel trafficModel = director.build();
    trafficModel.display();
    return 0;
}
