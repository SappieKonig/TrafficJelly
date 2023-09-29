#ifndef ROUTE_H
#define ROUTE_H

#include <random>
#include <vector>
#include "utils.h"

/*
 * This is a checkpoint for route planners to aim at and drive strategies to respond to.
 */
struct Checkpoint
{
    Node const& node;
    Edge& edgeToNode; // THIS IS A VEEEEERY UGLY SNEAKY WORKAROUND FOR TRANSFERRING NODES. TODO: FIX.
    float const waitDurationBeforeEdge;

    Checkpoint(Node const& node, Edge& edgeToNode, float const waitDurationBeforeEdge);
};

/*
 * This is a route of checkpoints for route planners to follow.
 * If the route is finished, the route planner will have arrived at the destination.
 */
struct Route
{
    std::vector<Checkpoint> checkpoints;
    void show();
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
