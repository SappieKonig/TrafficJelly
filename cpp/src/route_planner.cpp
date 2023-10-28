#include "route_planner.h"
#include "node.h"

RoutePlanner::~RoutePlanner()
{

}

BasicRoutePlanner::BasicRoutePlanner(Route& route)
    : route(route), targetCheckpoint(std::make_unique<Checkpoint>(route.checkpoints[0]))
{

}

bool BasicRoutePlanner::hasTerminated() const
{
    return checkpointIndex == route.checkpoints.size();
}

void BasicRoutePlanner::nextCheckpoint()
{
    targetCheckpoint = checkpointIndex < route.checkpoints.size() ? std::make_unique<Checkpoint>(route.checkpoints[checkpointIndex]) : nullptr;
    checkpointIndex++;
}

std::unique_ptr<Checkpoint>& BasicRoutePlanner::getTargetCheckpoint()
{
    return targetCheckpoint;
}

void BasicRoutePlanner::showRoute()
{
    route.show();
}
