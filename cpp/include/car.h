#ifndef CAR_H
#define CAR_H

#include "edge/basic_road/basic_road_dynamics.h"
#include <random>
#include <memory>
#include "utils.h"
#include "action.h"

class CruiseAction;
class HardBrakeAction;
class ToLeftLaneAction;
class ToRightLaneAction;
class CompositeAction;

/*
 * This is a car model for the traversal of the internal graph of TrafficModel.
 * They are the primary objects kept track of.
 * Each Car instance follows a route planner to its destination.
 * Cars remain idle on destination.
 */
class Car
{
private:
    float x = 0; // in m
    float baseTarget = 0; // in m/s
    float v; // in m/s (starts at speed limit of current edge (or maybe not, idfk))
    float offset = 0; // in m/s (to be added to v, with respect to the speed limit)
    int lane; // in number of lanes (starts at 0) (lane 0 is the rightmost lane)

    std::unique_ptr<Action> action = nullptr;
    BasicRoadDynamics dynamics = {};

    // Decision maker for graph traversal
//    std::unique_ptr<RoutePlanner> routePlanner;

public:
//    Car(std::unique_ptr<RoutePlanner> routePlanner);
    Car();
    void syncCarToEdge(float targetSpeed) {
        baseTarget = targetSpeed;
        x = 0;
    }
    void updateAction(Observation const& observation);
    void step(float dt);

    float getX() const { return x; }
    float getV() const { return v; }
    int getLane() const { return lane; }
    float getMargin() const { return 20 + 35 * v / 30; }

//    std::shared_ptr<Checkpoint> nextCheckpoint();
//    std::shared_ptr<Checkpoint> getTargetCheckpoint();
    // Friend all actions that need internal parameters
    friend CruiseAction;
    friend HardBrakeAction;
    friend ToLeftLaneAction;
    friend ToRightLaneAction;
    friend CompositeAction;

    float getTarget() const { return baseTarget + offset; }

    void accelerate(float dt);
    void softBrake(float dt);
    void hardBrake(float dt);

    void toLeftLane();
    void toRightLane();

    // Necessary for data-locality motivated sorting of the cars
    friend bool operator<(Car const& left, Car const& right);

    static std::default_random_engine generator;

    static std::normal_distribution<double> normalDistribution;
};

/*
 * This car factory interface acts as a source of cars to the system and may or may not be attached to a node.
 * The car factory may also help in deciding the distribution of properties, i.e. routes.
 * We may also decide to make Car abstract and split it into trucks and other stuff with different functionality.
 * Then, the car factory may have creation methods for such objects.
 */
//class CarFactory
//{
//public:
//    virtual std::unique_ptr<Car> createCar() = 0;
//};
//
//class BasicCarFactory : public CarFactory
//{
//private:
//    RouteSampler routeSampler;
//    std::default_random_engine generator;
//
//public:
//    BasicCarFactory(std::vector<Route>& routes, std::random_device& seed);
//    std::unique_ptr<Car> createCar() override;
//};

#endif
