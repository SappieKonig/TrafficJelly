#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

struct Car;
struct Node;
struct Edge;

/*
 * This is a checkpoint for route planners to aim at and drive strategies to respond to.
 */
struct Checkpoint
{
    Node const& node;
    Edge const& edgeToNode;
    float const waitDurationBeforeEdge;

    Checkpoint(Node const& node, Edge const& edgeToNode, float const waitDurationBeforeEdge);
};

/*
 * This is a route of checkpoints for route planners to follow.
 * If the route is finished, the route planner will have arrived at the destination.
 */
struct Route
{
    std::vector<Checkpoint> checkpoints; // Maybe also how long to spend in a city.
};

#endif
