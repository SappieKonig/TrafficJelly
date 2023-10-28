#ifndef ROUTE_H
#define ROUTE_H

#include <memory>
#include <random>
#include <vector>
#include "utils.h"

/*
 * This is a checkpoint for route planners to aim at and drive strategies to respond to.
 */
struct Checkpoint
{
    std::shared_ptr<Node const> const node;
    std::shared_ptr<Edge const> const edgeToNode;
    float const waitDurationBeforeEdge;

    Checkpoint(std::shared_ptr<Node const> const node, std::shared_ptr<Edge const> const edgeToNode, float const waitDurationBeforeEdge);
};

/*
 * This is a route of checkpoints for route planners to follow.
 * If the route is finished, the route planner will have arrived at the destination.
 */
struct Route
{
    std::vector<Checkpoint> checkpoints;
    void show() const;
    std::string toString() const;
};

/*
 * This is a sampler of routes for cars to follow.
 */
class RouteSampler
{
private:
    std::random_device device;
    std::default_random_engine generator;
    std::vector<Route>& routes;

public:
    RouteSampler(std::vector<Route>& routes, std::random_device& device);
    Route& select();
    void showRoutes();
};


#endif
