#include "route.h"
#include "edge.h"
#include "node.h"
#include <memory>

Checkpoint::Checkpoint(std::shared_ptr<Node const> const node, std::shared_ptr<Edge> const edgeToNode, float const waitDurationBeforeEdge)
    : node(node), edgeToNode(edgeToNode), waitDurationBeforeEdge(waitDurationBeforeEdge)
{

}

void Route::show() const// Those dashes are kind of ugly
{
    for (Checkpoint const& checkpoint : checkpoints)
    {
        std::cout << "-Road " << checkpoint.edgeToNode->getLabel() << "\n";
        std::cout << "-City " << checkpoint.node->getLabel() << "\n";
    }
}

std::string Route::toString() const
{
    std::string times;
    std::string nodes;
    std::string edges;
    for (int i = 1; i < checkpoints.size(); i++)
    {
        times += "," + std::to_string(checkpoints[i].waitDurationBeforeEdge);
        nodes += "," + checkpoints[i].node->getLabel();
        edges += "," + checkpoints[i].edgeToNode->getLabel();
    }

    std::string str;
    str += "RouteTimes:" + std::to_string(checkpoints[0].waitDurationBeforeEdge) + times + "\n";
    str += "RouteNodes:" + checkpoints[0].node->getLabel() + nodes + "\n";
    str += "RouteEdges:" + checkpoints[0].edgeToNode->getLabel() + edges + "\n";
    str += "Route:";
    return str;
}

RouteSampler::RouteSampler(std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device)
    : routes(routes), generator((*device)())
{

}

Route& RouteSampler::select()
{
    std::uniform_int_distribution<int> dist(0, routes->size()-1);
    int routeIndex = dist(generator);
    return routes->at(routeIndex);
}

void RouteSampler::showRoutes()
{
    for (int i = 0; i < routes->size(); i++)
    {
        std::cout << "Route " << i+1 << ":\n";
        routes->at(i).show();
    }
}
