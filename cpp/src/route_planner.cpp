#include "route_planner.h"

BasicRoutePlanner::BasicRoutePlanner(Route const& route)
    : route(route)
{

}

std::optional<std::reference_wrapper<Checkpoint const>> BasicRoutePlanner::nextCheckpoint()
{
    std::optional<std::reference_wrapper<Checkpoint const>> nextEdge = checkpointIndex < route.checkpoints.size() ? std::optional<std::reference_wrapper<Checkpoint const>>(route.checkpoints[checkpointIndex]) : std::nullopt;
    checkpointIndex++;
    return nextEdge;
}

