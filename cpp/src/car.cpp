#include "car.h"
//#include "route_planner.h"
//#include <memory>
#include <memory>
#include <random>
#include <iostream>
//#include <vector>
//#include "edge.h"

//std::default_random_engine Car::generator(time(0));
//std::normal_distribution<double> Car::normalDistribution(30, 3);

Car::Car(CarParameters params, std::unique_ptr<RoutePlanner> routePlanner) // Init this correctly.
    : params(std::move(params)), routePlanner(std::move(routePlanner))
{

}

void Car::step(float dt)
{
    if (action != nullptr) {
        action->apply(*this, dt);
    }
    params.x += params.v * dt;
}

void Car::accelerate(float dt)
{
    params.v += 2 * dt;
}

void Car::softBrake(float dt)
{
    params.v -= 2 * dt;
    if (params.v < 0) {
        params.v = 0;
    }
}

void Car::hardBrake(float dt)
{
    params.v -= 10 * dt;
    if (params.v < 0) {
        params.v = 0;
    }
}

void Car::toLeftLane()
{
    params.lane++;
}

void Car::toRightLane()
{
    params.lane--;
}

void Car::updateAction(Observation const& observation)
{
    action = dynamics.getAction(observation, *this);
}

bool Car::hasTerminated() const
{
    return routePlanner->hasTerminated();
}

void Car::nextCheckpoint()
{
    routePlanner->nextCheckpoint();
}

std::unique_ptr<Checkpoint>& Car::getTargetCheckpoint()
{
    return routePlanner->getTargetCheckpoint();
}

bool operator<(Car const& left, Car const& right)
{
    return left.params.x < right.params.x;
}

BasicCarFactory::BasicCarFactory(std::shared_ptr<std::vector<Route>> routes, std::shared_ptr<std::random_device> device)
    : routeSampler(routes, device), generator((*device)())
{

}

std::unique_ptr<Car> BasicCarFactory::createCar()
{
    //routeSampler.showRoutes();
    Route& route = routeSampler.select();
    CarParameters params;
    CarParametersInitializer(1, .5)(params, generator);
    return std::make_unique<Car>(params, std::make_unique<BasicRoutePlanner>(route));
};

