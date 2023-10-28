#ifndef TRAFFICJELLY_BASIC_ROAD_DYNAMICS_H
#define TRAFFICJELLY_BASIC_ROAD_DYNAMICS_H

#include "basic_road_observation.h"
#include "action.h"

class Action;
class Observation;

class BasicRoadDynamics {
public:
    std::unique_ptr<Action> getAction(Observation const& observation, Car const& ego);
};

#endif
