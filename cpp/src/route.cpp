#include "route.h"

Checkpoint::Checkpoint(Node const& node, Edge const& edgeToNode, float waitDurationBeforeEdge)
    : node(node), edgeToNode(edgeToNode), waitDurationBeforeEdge(waitDurationBeforeEdge)
{

}

