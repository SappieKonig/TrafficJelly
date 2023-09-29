#include <fstream>

#include "traffic_model.h"
#include <iostream>

// TODO: CHECK IF SAVES ROUTES AS WELL!
int main()
{
    float dt = 0.1f;
    TrafficModelBuilder trafficModelBuilder;
    trafficModelBuilder.addBasicCity("Amsterdam");
    trafficModelBuilder.addBasicCity("Utrecht");
    trafficModelBuilder.addBasicCity("Delft");
    trafficModelBuilder.addBasicRoad("A2-1", "Utrecht", "Amsterdam", 40);
    trafficModelBuilder.addBasicRoad("A2-2", "Amsterdam", "Utrecht", 40);
    trafficModelBuilder.addBasicRoad("N446-1", "Delft", "Amsterdam", 70);
    trafficModelBuilder.addBasicRoad("N446-2", "Amsterdam", "Delft", 70);
    trafficModelBuilder.addBasicRoad("A12-1", "Utrecht", "Delft", 70);
    trafficModelBuilder.addBasicRoad("A12-2", "Delft", "Utrecht", 70);
    trafficModelBuilder.addRoute({"Amsterdam", "Utrecht"}, {"N446-1", "A2-2"}, {0, 1});
    trafficModelBuilder.save("tests/output/save_test.txt");
    trafficModelBuilder.build().showRoutes();
    return 0;
}
