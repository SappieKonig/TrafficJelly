#include "route.h"
#include "edge/edge.h"
#include "node/node.h"

Checkpoint::Checkpoint(Node const& node, Edge& edgeToNode, float waitDurationBeforeEdge) // Part of the very sneaky workaround
    : node(node), edgeToNode(edgeToNode), waitDurationBeforeEdge(waitDurationBeforeEdge)
{

}

void Route::show() // Those dashes are kind of ugly
{
    for (Checkpoint& checkpoint : checkpoints)
    {
        std::cout << "-Road " << checkpoint.edgeToNode.getLabel() << "\n";
        std::cout << "-City " << checkpoint.node.getLabel() << "\n";
    }
}

std::string Route::toString() const
{
    std::string times;
    std::string nodes;
    std::string edges;
    for (int i = 0; i < checkpoints.size() - 1; i++)
    {
        times += "," + std::to_string(checkpoints[i].waitDurationBeforeEdge);
        nodes += "," + checkpoints[i].node.getLabel();
        edges += "," + checkpoints[i].edgeToNode.getLabel();
    }

    std::string str;
    str += "RouteTimes:" + std::to_string(checkpoints[0].waitDurationBeforeEdge) + times + "\n";
    str += "RouteNodes:" + checkpoints[0].node.getLabel() + nodes + "\n";
    str += "RouteEdges:" + checkpoints[0].edgeToNode.getLabel() + edges + "\n";
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

