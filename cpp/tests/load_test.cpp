#include <fstream>
#include <iostream>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    TrafficModelFileDirector director("tests/input/load_test.txt");
    director.build().display();
    return 0;
}
