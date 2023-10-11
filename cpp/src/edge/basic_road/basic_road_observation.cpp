//
// Created by sappie on 10-10-23.
//

#include <cstdlib>
#include "edge/basic_road/basic_road_observation.h"

Observation::Observation(std::vector<std::reference_wrapper<Car>> const& cars,
                         std::unique_ptr<Car> const& ego,
                         int n_lanes)
{
    if (ego->getLane() == n_lanes - 1) {
        leftLaneExists = false;
    }
    if (ego->getLane() == 0) {
        rightLaneExists = false;
    }
    for (auto const& car : cars) {
        int dLane = car.get().getLane() - ego->getLane();
        float dX = car.get().getX() - ego->getX();
        float dV = car.get().getV() - ego->getV();
        if (abs(dLane) > 1) {
            continue;
        }
        RelativeCarObservation relativeCarObservation;
        if (dLane == 1) {
            if (dX > 0) {
                if (!left_front.has_value() || left_front->dx > dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    left_front = relativeCarObservation;
                }
            } else {
                if (!left_back.has_value() || left_back->dx < dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    left_back = relativeCarObservation;
                }
            }
        }
        else if (dLane == -1) {
            if (dX > 0) {
                if (!right_front.has_value() || right_front->dx > dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    right_front = relativeCarObservation;
                }
            } else {
                if (!right_back.has_value() || right_back->dx < dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    right_back = relativeCarObservation;
                }
            }
        }
        else {
            if (dX > 0) {
                if (!front.has_value() || front->dx > dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    front = relativeCarObservation;
                }
            } else {
                if (!back.has_value() || back->dx < dX) {
                    relativeCarObservation.dx = dX;
                    relativeCarObservation.dv = dV;
                    back = relativeCarObservation;
                }
            }
        }
    }
}
