#include "car.h"

Car::Car(std::unique_ptr<RoutePlanner> routePlanner, std::unique_ptr<ActionGenerator> actionGenerator, std::unique_ptr<ActionManager> actionManager)
    : routePlanner(std::move(routePlanner)), actionGenerator(std::move(actionGenerator)), actionManager(std::move(actionManager))
{

}

void Car::step(Observation const& observation, float dt)
{
    ;
}

void Car::nextCheckpoint()
{
    optional_const_reference<Checkpoint> checkpoint = routePlanner->nextCheckpoint();
    actionGenerator->setCheckpoint(checkpoint);
}

float Car::getX() const
{
    return x;
}

bool operator<(Car const& left, Car const& right)
{
    return left.x < right.x;
}

