#include "car.h"
#include <random>


std::default_random_engine Car::generator(time(0));
std::normal_distribution<double> Car::normalDistribution(0, 3);


Car::Car(std::vector<int> path, float global_time, float scale, int carID) :
    path(path), global_time(global_time), scale(scale), carID(carID)
{
    fromNodeID = path[0];
    toNodeID = path[path.size() - 1];
    v = 20;
    // offset random number between -5 and 5
    offset = (float) (normalDistribution(generator));
    if (offset < -5) {
        offset = -5;
    } else if (offset > 5) {
        offset = 5;
    }
    lane = 0;
}

void Car::step(float dt)
{
    if (action != nullptr) {
        action->apply(*this, dt);
    }
    x += v * dt;
    age += dt / scale;
}

void Car::accelerate(float dt)
{
    v += 2 * dt;
}

void Car::softBrake(float dt)
{
    v -= 2 * dt;
    if (v < 0) {
        v = 0;
    }
}

void Car::hardBrake(float dt)
{
    v -= 10 * dt;
    if (v < 0) {
        v = 0;
    }
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

