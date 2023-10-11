#include "route_planner.h"
#include "node/node.h"

RoutePlanner::~RoutePlanner()
{

}

BasicRoutePlanner::BasicRoutePlanner(Route& route)
    : route(route)
{

}

std::shared_ptr<Checkpoint> BasicRoutePlanner::nextCheckpoint()
{
    std::shared_ptr<Checkpoint> nextCheckpoint = checkpointIndex < route.checkpoints.size() ? std::make_unique<Checkpoint>(route.checkpoints[checkpointIndex]) : nullptr;
    checkpointIndex++;
    return nextCheckpoint;
}

void BasicRoutePlanner::showRoute()
{
    route.show();
}
