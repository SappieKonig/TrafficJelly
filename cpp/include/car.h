#ifndef CAR_H
#define CAR_H

#include "basic_road_dynamics.h"
#include <random>
#include <memory>
#include "car_parameters.h"
#include "utils.h"
#include "route_planner.h"
#include "action.h"
//#include "observation.h"

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
    CarParameters params;
    std::unique_ptr<Action> action = nullptr;
    BasicRoadDynamics dynamics = {};

    // Decision maker for graph traversal
    std::unique_ptr<RoutePlanner> routePlanner;

public:
    Car(CarParameters params, std::unique_ptr<RoutePlanner> routePlanner);
    void syncCarToEdge(float targetSpeed) {
        params.baseTarget = targetSpeed;
        params.x = 0;
    }
    void updateAction(Observation const& observation);
    void step(float dt);

    float getX() const { return params.x; }
    float getV() const { return params.v; }
    int getLane() const { return params.lane; }
    float getMargin() const { return 20 + 35 * params.v / 30; }

    bool hasTerminated() const;
    void nextCheckpoint();
    std::unique_ptr<Checkpoint>& getTargetCheckpoint();

    // Friend all actions that need internal parameters
    friend CruiseAction;
    friend HardBrakeAction;
    friend ToLeftLaneAction;
    friend ToRightLaneAction;
    friend CompositeAction;

    float getTarget() const { return params.baseTarget + params.offset; }

    void accelerate(float dt);
    void softBrake(float dt);
    void hardBrake(float dt);

    void toLeftLane();
    void toRightLane();

    // Necessary for data-locality motivated sorting of the cars
    friend bool operator<(Car const& left, Car const& right);
};

/*
 * This car factory interface acts as a source of cars to the system and may or may not be attached to a node.
 * The car factory may also help in deciding the distribution of properties, i.e. routes.
 * We may also decide to make Car abstract and split it into trucks and other stuff with different functionality.
 * Then, the car factory may have creation methods for such objects.
 */
class CarFactory
{
public:
    virtual std::unique_ptr<Car> createCar() = 0;
};

class BasicCarFactory : public CarFactory
{
private:
    RouteSampler routeSampler;
    std::default_random_engine generator;

public:
    BasicCarFactory(std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device);
    std::unique_ptr<Car> createCar() override;
};

#endif
