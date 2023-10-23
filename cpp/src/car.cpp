#include "car.h"
//#include "route_planner.h"
//#include <memory>
#include <random>
#include <iostream>
//#include <vector>
//#include "edge/edge.h"


std::default_random_engine Car::generator(time(0));
std::normal_distribution<double> Car::normalDistribution(30, 3);


Car::Car()
{
    v = (float) normalDistribution(generator);
    offset = 0;
    lane = 0;
}

void Car::step(float dt)
{
    if (action != nullptr) {
        action->apply(*this, dt);
    }
    x += v * dt;
}

void Car::accelerate(float dt)
{
    v += 2 * dt;
}

void Car::softBrake(float dt)
{
    v -= 2 * dt;
}

void Car::hardBrake(float dt)
{
    v -= 10 * dt;
}

void Car::toLeftLane()
{
    lane++;
}

void Car::toRightLane()
{
    lane--;
}

void Car::updateAction(const Observation &observation) {
    action = dynamics.getAction(observation, *this);
}

//std::shared_ptr<Checkpoint> Car::nextCheckpoint()
//{
//    std::shared_ptr<Checkpoint> targetCheckpoint = routePlanner->nextCheckpoint();
//    params->targetCheckpoint = targetCheckpoint;
//    return targetCheckpoint;
//}
//
//std::shared_ptr<Checkpoint> Car::getTargetCheckpoint()
//{
//    return params->targetCheckpoint;
//}

bool operator<(Car const& left, Car const& right)
{
    return left.x < right.x;
}

//BasicCarFactory::BasicCarFactory(std::vector<Route>& routes, std::random_device& device)
//    : routeSampler(routes, device), generator(device())
//{
//
//}

//std::unique_ptr<Car> BasicCarFactory::createCar()
//{
//    Route& route = routeSampler.select();
//    routeSampler.showRoutes();
//    std::shared_ptr<CarParameters> params = std::make_shared<CarParameters>();
//    CarParametersInitializer(0, 0.1, 1, .5)(params, generator);
//    return std::make_unique<Car>(params, std::make_unique<BasicActionManager>(params), std::make_unique<BasicRoutePlanner>(route), std::make_unique<BasicActionGenerator>(params));
//};
