#include <fstream>

#include "traffic_model.h"

int main()
{
    float dt = 0.1f;
    TrafficModelBuilder trafficModelBuilder;
    trafficModelBuilder.addBasicCity("Amsterdam");
    trafficModelBuilder.addBasicCity("Utrecht");
    trafficModelBuilder.addBasicCity("Delft");
    trafficModelBuilder.addBasicRoad("Utrecht", "Amsterdam", "A2", 40);
    trafficModelBuilder.addBasicRoad("Amsterdam", "Utrecht", "A2", 40);
    trafficModelBuilder.addBasicRoad("Delft", "Amsterdam", "N446", 70);
    trafficModelBuilder.addBasicRoad("Amsterdam", "Delft", "N446", 70);
    trafficModelBuilder.addBasicRoad("Utrecht", "Delft", "A12", 70);
    trafficModelBuilder.addBasicRoad("Delft", "Utrecht", "A12", 70);
    trafficModelBuilder.save("tests/output/save_test.txt");
    return 0;
}
