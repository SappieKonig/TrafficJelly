#include "route.h"
#include "edge.h"
#include "node.h"
#include <memory>

Checkpoint::Checkpoint(std::shared_ptr<Node const> const node, std::shared_ptr<Edge const> const edgeToNode, float const waitDurationBeforeEdge)
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
    int end = checkpoints.size() - 1;
    std::string times;
    std::string nodes;
    std::string edges;
    for (int i = 0; i < end; i++)
    {
        times += "," + std::to_string(checkpoints[i].waitDurationBeforeEdge);
        nodes += "," + checkpoints[i].node->getLabel();
        edges += "," + checkpoints[i].edgeToNode->getLabel();
    }

    std::string str;
    str += "RouteTimes:" + std::to_string(checkpoints[end].waitDurationBeforeEdge) + times + "\n";
    str += "RouteNodes:" + checkpoints[end].node->getLabel() + nodes + "\n";
    str += "RouteEdges:" + checkpoints[end].edgeToNode->getLabel() + edges + "\n";
    str += "Route:";
    return str;
}

RouteSampler::RouteSampler(std::vector<Route>& routes, std::random_device& device)
    : routes(routes), generator(device())
{

}

Route& RouteSampler::select()
{
    std::uniform_int_distribution<int> dist(0, routes.size()-1);
    int routeIndex = dist(generator);
    return routes[routeIndex];
}

void RouteSampler::showRoutes()
{
    std::cout << routes.size() << "\n";
    for (int i = 0; i < routes.size(); i++)
    {
        std::cout << "Route " << i+1 << ":\n";
        routes[i].show();
    }
}

