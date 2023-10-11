
//
// Created by sappie on 10-10-23.
//

#ifndef TRAFFICJELLY_BASIC_ROAD_DYNAMICS_H
#define TRAFFICJELLY_BASIC_ROAD_DYNAMICS_H


#include "edge/basic_road/basic_road_observation.h"
#include "action.h"


class Action;
class Observation;

class BasicRoadDynamics {
public:
    std::unique_ptr<Action> getAction(Observation const& observation,
                                      Car const& ego);
};


#endif //TRAFFICJELLY_BASIC_ROAD_DYNAMICS_H
