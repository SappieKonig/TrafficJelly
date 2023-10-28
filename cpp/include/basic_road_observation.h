//
// Created by sappie on 10-10-23.
//
// Observation for a simple road. In a certain range around the car
// (n meters, implemented in BasicRoad itself) the cars are observed.
// That is, speed, position. (to possibly be extended with braking, etc.)

#ifndef TRAFFICJELLY_OBSERVATION_H
#define TRAFFICJELLY_OBSERVATION_H

#include <vector>
#include <optional>
#include <memory>

class Car;

class RelativeCarObservation{
public:
    float dx, dv;
};


class Observation {
public:
    Observation(std::vector<std::reference_wrapper<Car>> const& cars,
                std::unique_ptr<Car> const& ego,
                int n_lanes);
    bool leftLaneExists = true;
    bool rightLaneExists = true;
    std::optional<RelativeCarObservation> left_front = std::nullopt;
    std::optional<RelativeCarObservation> left_back = std::nullopt;
    std::optional<RelativeCarObservation> right_front = std::nullopt;
    std::optional<RelativeCarObservation> right_back = std::nullopt;
    std::optional<RelativeCarObservation> front = std::nullopt;
    std::optional<RelativeCarObservation> back = std::nullopt;
};


#endif //TRAFFICJELLY_OBSERVATION_H
