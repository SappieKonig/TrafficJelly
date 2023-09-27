#ifndef CAR_H
#define CAR_H

#include <memory>
#include "utils.h"
#include "route_planner.h"
#include "action_generator.h"
#include "action_manager.h"
#include "observation.h"

/*
 * This is a car model for the traversal of the internal graph of TrafficModel.
 * They are the primary objects kept track of.
 * Each Car instance follows a route planner to its destination.
 * Cars remain idle on destination.
 */
struct Car
{
private:
    // Decision maker for graph traversal
    std::unique_ptr<RoutePlanner> routePlanner;

    // Decision maker for local behaviour
    std::unique_ptr<ActionGenerator> actionGenerator;

    // Actions to perform each iteration
    std::unique_ptr<ActionManager> actionManager;

    // Edge dynamics
    float x = 0; // in km
    float v = 0; // in km/h

public:
    Car(std::unique_ptr<RoutePlanner> routePlanner, std::unique_ptr<ActionGenerator> actionGenerator, std::unique_ptr<ActionManager> actionManager);
    void step(Observation const& observation, float dt);
    void nextCheckpoint();
    bool hasTerminated();
    float getX() const;

    // Friend all actions that need internal parameters
    friend DriveAction;
    friend WaitAction;
    friend EdgeChoiceAction;
    friend CompositeAction;

    // Necessary for data-locality motivated sorting of the cars
    friend bool operator<(Car const& left, Car const& right);
};

#endif
