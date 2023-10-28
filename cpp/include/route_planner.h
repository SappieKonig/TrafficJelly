#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include "route.h"
#include "utils.h"
#include <memory>

/*
 * This is a route planner interface for the choice that a car has to make, when exiting a node.
 * It decides on the edge that is next to be traversed.
 */
class RoutePlanner
{
public:
    virtual ~RoutePlanner();
    virtual bool hasTerminated() const = 0;
    virtual void nextCheckpoint() = 0;
    virtual std::unique_ptr<Checkpoint>& getTargetCheckpoint() = 0;
    virtual void showRoute() = 0;
};

/*
 * This is a basic route planner to choose checkpoints of a fixed route.
 * It does so by tracking the position of the car along a fixed route, and returning the next checkpoint, if present.
 * If the route planner has arrived at its destination (i.e. has no next checkpoint), the car is expected to terminate its journey.
 */
class BasicRoutePlanner : public RoutePlanner
{
private:
    Route& route; // Maybe extend to hold multiple routes, or allow for perturbations.
    int checkpointIndex = 0;
    std::unique_ptr<Checkpoint> targetCheckpoint;

public:
    BasicRoutePlanner(Route& route);
    bool hasTerminated() const override;
    void nextCheckpoint() override;
    std::unique_ptr<Checkpoint>& getTargetCheckpoint() override;
    void showRoute() override;
};

#endif
