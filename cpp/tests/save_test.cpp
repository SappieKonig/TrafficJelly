#include <fstream>

#include "traffic_model.h"
#include <iostream>

int main()
{
    float dt = 0.1f;
    TrafficModelBuilder trafficModelBuilder;
    trafficModelBuilder.addBasicCity("Amsterdam", 900000);
    trafficModelBuilder.addBasicCity("Utrecht", 360000);
    trafficModelBuilder.addBasicCity("Delft", 100000);
    trafficModelBuilder.addBasicRoad("A2-1", "Utrecht", "Amsterdam", 40, 100, 4);
    trafficModelBuilder.addBasicRoad("A2-2", "Amsterdam", "Utrecht", 40, 100, 4);
    trafficModelBuilder.addBasicRoad("A4-1", "Delft", "Amsterdam", 70, 100, 4);
    trafficModelBuilder.addBasicRoad("A4-2", "Amsterdam", "Delft", 70, 100, 4);
    trafficModelBuilder.addBasicRoad("A12-1", "Utrecht", "Delft", 70, 100, 4);
    trafficModelBuilder.addBasicRoad("A12-2", "Delft", "Utrecht", 70, 100, 4);
    trafficModelBuilder.addRoute({"Amsterdam", "Utrecht"}, {"A4-1", "A2-2"}, {0, 1});
    trafficModelBuilder.save("tests/output/save_test.txt");
    TrafficModel trafficModel = trafficModelBuilder.build(); // Internal trafficModel of trafficModelBuilder is reset!
    trafficModel.display();
    trafficModel = trafficModelBuilder.build(); // So, now we may build a completely new one.
    trafficModel.display(); // Note that this one is empty, because we did not build anything.
    return 0;
}
