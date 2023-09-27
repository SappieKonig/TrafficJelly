#include <fstream>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    TrafficModelFileDirector director("./tests/input/save_test.txt");
    TrafficModel trafficModel = director.build();
    for (int i = 0; i < 100; i++)
    {
        trafficModel.step(dt);
    }
    trafficModel.display();
    return 0;
}
