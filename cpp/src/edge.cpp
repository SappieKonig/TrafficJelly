#include "edge.h"
#include "node.h"

#include <iostream>

Edge::Edge(Node& inNode, Node& outNode, std::string label, float speedLimit)
    : inNode(inNode), outNode(outNode), label(std::move(label)), speedLimit(speedLimit)
{
    inNode.outEdges.emplace_back(*this);
    outNode.inEdges.emplace_back(*this);
    length = inNode.distanceTo(outNode);
    id = -1;
}

Edge::~Edge()
{

}

std::string Edge::getLabel() const
{
    return label;
}

void Edge::sortCars() {
    // We sort cars by iterating over them, and if the order is wrong, we swap them.
    // After a swap, we keep going with the car with the least distance,
    // because it may have been overtaken twice (or more).
    // Since we only have local changes, we can do this in O(n),
    // with practically zero memory cost.
    for (auto it = cars.begin(); it != cars.end() && std::next(it) != cars.end(); ) {
        auto next = std::next(it);
        if (it->get()->getX() > next->get()->getX()) {
            std::iter_swap(it, next);
            // Step back to re-check, unless we're at the beginning
            if (it != cars.begin()) {
                it = std::prev(it);
                continue;
            }
        }
        it = next;
    }
}


void Edge::updateCars(float dt)
{
    for (auto& car : cars)
    {
        car->step(dt);
    }
}

std::vector<std::unique_ptr<Car>> Edge::getExitingCars() {
    std::vector<std::unique_ptr<Car>> exiting_cars;
    // Loop from back to front
    int cars_removed = 0;
    for (auto car = cars.rbegin(); car != cars.rend(); ++car) {
        if ((*car)->getX() > length) {
            exiting_cars.push_back(std::move(*car));
            cars_removed++;
        }
        else {
            break;
        }
    }
    for (int i = 0; i < cars_removed; ++i) {
        cars.pop_back();
    }
    return exiting_cars;
}

std::tuple<std::vector<int>, std::vector<float>> Edge::getCarCountHist(float bin_distance) const
{
    std::vector<int> counts;
    std::vector<float> bins;
    float distance = 0;
    auto car = cars.begin();
    int count = 0;
    while (distance < length) {
        bins.push_back(distance);
        while (car != cars.end() && (*car)->getX() < distance + bin_distance) {
            count++;
            car++;
        }
        counts.push_back(count);
        count = 0;
        distance += bin_distance;
    }
    bins.push_back(length);
    return std::make_tuple(counts, bins);
}
