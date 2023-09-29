#include "car.h"
#include "route_planner.h"
#include <memory>
#include <random>
#include <vector>
#include "edge.h"

Car::Car(std::shared_ptr<CarParameters> params, std::unique_ptr<ActionManager> actionManager, std::unique_ptr<RoutePlanner> routePlanner, std::unique_ptr<ActionGenerator> actionGenerator)
    : params(std::move(params)), actionManager(std::move(actionManager)), routePlanner(std::move(routePlanner)), actionGenerator(std::move(actionGenerator))
{
    auto ch = this->routePlanner->nextCheckpoint();
    this->params->targetCheckpoint = ch;
}

void Car::step(Observation const& observation, float dt)
{
    std::unique_ptr<Action> action = actionGenerator->generateAction(observation);
    actionManager->manage(std::move(action));
    actionManager->step(dt);
}

std::shared_ptr<Checkpoint> Car::nextCheckpoint()
{
    std::shared_ptr<Checkpoint> targetCheckpoint = routePlanner->nextCheckpoint();
    params->targetCheckpoint = targetCheckpoint;
    return targetCheckpoint;
}

float Car::getX() const
{
    return params->x;
}

bool Car::isWaiting() const
{
    return params->isWaiting;
}

std::shared_ptr<Checkpoint> Car::getTargetCheckpoint()
{
    return params->targetCheckpoint;
}

bool operator<(Car const& left, Car const& right)
{
    return left.params->x < right.params->x;
}

BasicCarFactory::BasicCarFactory(std::vector<Route>& routes, std::random_device& device)
    : routeSampler(routes, device), generator(device())
{

}

std::unique_ptr<Car> BasicCarFactory::createCar()
{
    Route& route = routeSampler.select();
    routeSampler.showRoutes();
    std::shared_ptr<CarParameters> params = std::make_shared<CarParameters>();
    CarParametersInitializer(0, 0.1, 1, .5)(params, generator);
    return std::make_unique<Car>(params, std::make_unique<BasicActionManager>(params), std::make_unique<BasicRoutePlanner>(route), std::make_unique<BasicActionGenerator>(params));
};

