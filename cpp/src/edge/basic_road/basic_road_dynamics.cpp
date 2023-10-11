//
// Created by sappie on 10-10-23.
//

#include <vector>

#include "action.h"
#include "edge/basic_road/basic_road_dynamics.h"



std::unique_ptr<Action> BasicRoadDynamics::getAction(const Observation &observation,
                                                     const Car &ego) {
    float margin = ego.getMargin();
    // If our lane is not the right line, we want to see
    // if we can get into the right lane
    if (observation.rightLaneExists) {
        bool front_safe = !observation.right_front.has_value() ||
                (observation.right_front->dx > margin &&
                 observation.right_front->dv > -10);
        bool back_safe = !observation.right_back.has_value() ||
                (observation.right_back->dx < -margin &&
                 observation.right_back->dv < 10);
        if (front_safe && back_safe) {
            std::vector<std::unique_ptr<Action>> actions;
            actions.push_back(std::make_unique<ToRightLaneAction>());
            actions.push_back(std::make_unique<CruiseAction>());
            return std::make_unique<CompositeAction>(std::move(actions));
        }
    }

    // If there is no right lane or we can't get into it,
    // we want to try and keep cruising
    if (!observation.front.has_value() ||
            (observation.front->dx > margin &&
             observation.front->dv > -10)) {
        return std::make_unique<CruiseAction>();
    }

    // If we can't cruise, we want to try and get into the left lane
    if (observation.leftLaneExists) {
        bool front_safe = !observation.left_front.has_value() ||
                (observation.left_front->dx > margin &&
                 observation.left_front->dv > -10);
        bool back_safe = !observation.left_back.has_value() ||
                (observation.left_back->dx < -margin &&
                 observation.left_back->dv < 10);
        if (front_safe && back_safe) {
            std::vector<std::unique_ptr<Action>> actions;
            actions.push_back(std::make_unique<ToLeftLaneAction>());
            actions.push_back(std::make_unique<CruiseAction>());
            return std::make_unique<CompositeAction>(std::move(actions));
        }
    }

    // If we can't get into the left lane, we slow down
    return std::make_unique<HardBrakeAction>();
}
