#ifndef CAR_H
#define CAR_H

#include <random>
#include <memory>
#include "utils.h"
#include "route_planner.h"
#include "action_generator.h"
#include "action_manager.h"
#include "observation.h"
#include "car_parameters.h"

/*
 * This is a car model for the traversal of the internal graph of TrafficModel.
 * They are the primary objects kept track of.
 * Each Car instance follows a route planner to its destination.
 * Cars remain idle on destination.
 */
class Car
{
private:
    std::shared_ptr<CarParameters> params;

    // Actions to perform each iteration
    std::unique_ptr<ActionManager> actionManager;

    // Decision maker for graph traversal
    std::unique_ptr<RoutePlanner> routePlanner;

    // Decision maker for local behaviour
    std::unique_ptr<ActionGenerator> actionGenerator;

public:
    Car(std::shared_ptr<CarParameters> params, std::unique_ptr<ActionManager> actionManager, std::unique_ptr<RoutePlanner> routePlanner, std::unique_ptr<ActionGenerator> actionGenerator);
    void step(Observation const& observation, float dt);
    std::shared_ptr<Checkpoint> nextCheckpoint();
    float getX() const;
    bool isWaiting() const;
    std::shared_ptr<Checkpoint> getTargetCheckpoint();

    // Friend all actions that need internal parameters
    friend DriveAction;
    friend WaitAction;
    friend CompositeAction;

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
    BasicCarFactory(std::vector<Route>& routes, std::random_device& seed);
    std::unique_ptr<Car> createCar() override;
};

#endif
